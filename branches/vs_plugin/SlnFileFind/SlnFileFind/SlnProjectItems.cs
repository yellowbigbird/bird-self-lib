using System;
using System.Diagnostics;
using System.Collections;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.ComponentModel;
using System.IO;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using EnvDTE;
using EnvDTE80;

namespace SlnFileFindAddIn
{
    class SlnProjectItem
    {
        public SlnProjectItem(ProjectItem item, int iconIdx)
        {
            fileName = item.Name;
            projectName = item.ContainingProject.Name;
            fullPath = item.get_FileNames(1);   // VB indexing starts with 1

            iconIndex = iconIdx;
        }

        public void AddProjectName(string name)
        {
            if (projectName.Length > 0)
            {
                string[] names = projectName.Split(
                    new string[] { nameDelim }, StringSplitOptions.RemoveEmptyEntries);

                foreach (string s in names)
                    if (s == name) return;  // The name is there already.

                projectName += nameDelim + name;
            }
            else projectName = name;
        }

        public bool MultipleProjects
        {
            get { return projectName.Contains(nameDelim); }
        }

        public string ProjectName
        {
            get
            {
                return this.MultipleProjects ?
                    "(Multiple projects)" : projectName;
            }
        }

        public string FullProjectName
        {
            get { return projectName; }
        }

        public string fileName;
        private string projectName;
        public string fullPath;
      
        public int iconIndex;

        private const string nameDelim = "\r\n";
    }

    enum SlnProjectItemOrder
    {
        ByItemName,
        ByProjectName,
        ByItemFullPath
    }

    struct SlnProjectItemCompare : IComparer<SlnProjectItem>
    {
        public SlnProjectItemCompare(
            SortOrder sortOrder, SlnProjectItemOrder itemOrder)
        {
            _sortOrder = sortOrder;
            _itemOrder = itemOrder;
        }

        #region IComparer<SlnProjectItem> Members

        public int Compare(SlnProjectItem x, SlnProjectItem y)
        {
            SlnProjectItem item1 =
                (_sortOrder == SortOrder.Ascending ? x : y);
            SlnProjectItem item2 =
                (_sortOrder == SortOrder.Ascending ? y : x);

            int result;
            switch (_itemOrder)
            {
                case SlnProjectItemOrder.ByItemName:
                    result = item1.fileName.CompareTo(item2.fileName);
                    break;
                case SlnProjectItemOrder.ByProjectName:
                    result = item1.ProjectName.CompareTo(item2.ProjectName);
                    break;
                case SlnProjectItemOrder.ByItemFullPath:
                    result = item1.fullPath.CompareTo(item2.fullPath);
                    break;
                default:
                    throw new Exception("Unknown item ordering. Fix and rebuild.");
            }

            return result;
        }

        #endregion

        SortOrder _sortOrder;
        SlnProjectItemOrder _itemOrder;
    }

    class SlnProjectItems
    {
        #region Public interface

        public SlnProjectItems()
        {
            _knownExtensions = new Dictionary<string, int>(
                StringComparer.InvariantCultureIgnoreCase);

            Shell32.SHGetImageList((int)SHIL.SHIL_SYSSMALL,
                ref Shell32.IID_IImageList, out _hSystemImageListSmall);

            _projItems = new ProjectItemsCollection();

            _bkgWorker = new BackgroundWorker();
            _bkgWorker.WorkerReportsProgress = true;
            _bkgWorker.WorkerSupportsCancellation = true;
            _bkgWorker.DoWork += new DoWorkEventHandler(
                this.BkgWorker_DoWork);
            _bkgWorker.RunWorkerCompleted += new RunWorkerCompletedEventHandler(
                this.BkgWorker_RunWorkerCompleted);

            // Default compare - ascending, compare file names.
            _activeCompare = new SlnProjectItemCompare(
                SortOrder.Ascending, SlnProjectItemOrder.ByItemName);

            _pendingFilter = null;
        }

        public delegate void CollectionChangedHandler();
        public event CollectionChangedHandler CollectionChanged;

        /// <summary> 
        /// Sort with arbitrary predicate.
        /// </summary> 
        /// <param name="comparison">The SlnProjectItemCompare to use when comparing elements.</param>
        public bool Sort(SlnProjectItemCompare comparison)
        {
            if (_bkgWorker.IsBusy)
                return false;

            _activeCompare = comparison;
            SortItemsToShow(_projItems, _activeCompare);
            NotifyCollectionChanged();

            return true;
        }

        /// <summary> 
        /// Removes all elements from the item's collection.
        /// </summary> 
        public void Clear()
        {
            if (_bkgWorker.IsBusy)
            {
                _bkgWorker.CancelAsync();
                return;
            }

            ClearProjectItems();
            NotifyCollectionChanged();
        }

        /// <summary> 
        /// Applies filter substring to the items list.
        /// </summary>
        /// <param name="filter">Filter substring to apply for each item in the list.</param>
        public void ApplyFilter(string filter)
        {
            if (_bkgWorker.IsBusy)
            {
                _pendingFilter = filter;
                return;
            }

            _pendingFilter = null;

            FilterProjectItems(_projItems, filter);
            SortItemsToShow(_projItems, _activeCompare);
            NotifyCollectionChanged();
        }

        /// <summary> 
        /// Gets the element at the specified index.
        /// </summary>
        /// <param name="index">The zero-based index of the element to get.</param>
        public SlnProjectItem this[int index]
        {
            get
            {
                int realIndex = _projItems._itemsToShow[index];

                return _projItems._items[realIndex];
            }
        }

        /// <summary> 
        /// The number of elements actually contained in the items list.
        /// </summary>
        /// <param name="index">The zero-based index of the element to get.</param>
        public int Count
        {
            get { return _projItems._itemsToShow.Count; }
        }

        /// <summary>
        /// 
        /// </summary>
        public IntPtr SmallImageListHandle
        {
            get { return _hSystemImageListSmall; }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="applicationObject"></param>
        public void RetrieveSolutionItems(DTE2 applicationObject)
        {
            if (_bkgWorker.IsBusy)
                return;

           _bkgWorker.RunWorkerAsync(applicationObject);
        }

        #endregion  // Public interface

        void BkgWorker_DoWork(object sender, DoWorkEventArgs e)
        {
            ProjectItemsCollection projectItems = new ProjectItemsCollection();

            BackgroundWorker worker = (BackgroundWorker)sender;
            DTE2 applicationObject = (DTE2)e.Argument;

            RetrieveSolutionItems(projectItems, applicationObject, worker, e);

            projectItems._items.Sort(_activeCompare);

            FilterProjectItems(projectItems, _pendingFilter);
            _pendingFilter = null;

            SortItemsToShow(projectItems, _activeCompare);

            e.Result = projectItems;
        }

        // Note: This handler is called in the context of worker thread.
        void BkgWorker_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            lock (SlnFileFind.ProjectItems)
            {
                if (e.Cancelled || e.Error != null)
                    ClearProjectItems();
                else // success
                    _projItems = (ProjectItemsCollection)e.Result;
            }

            NotifyCollectionChanged();
        }

        void RetrieveSolutionItems(
            ProjectItemsCollection projectItems,
            DTE2 applicationObject,
            BackgroundWorker worker,
            DoWorkEventArgs e)
        {
            // Accumulate project items in case insensitive dictionary to
            // avoid duplicates. The key is item's path.
            Dictionary<string, SlnProjectItem> dic =
                new Dictionary<string, SlnProjectItem>(
                    StringComparer.InvariantCultureIgnoreCase);

            foreach (Project project in applicationObject.Solution.Projects)
            {
                RetrieveProjectItems(project.ProjectItems, worker, e, dic);
            }

            projectItems._items.AddRange(dic.Values);
        }

        void RetrieveProjectItems(
            ProjectItems projectItems,
            BackgroundWorker worker,
            DoWorkEventArgs e,
            Dictionary<string, SlnProjectItem> dic)
        {
            if (projectItems == null) return;

            foreach (ProjectItem item in projectItems)
            {
                if (worker.CancellationPending)
                {
                    e.Cancel = true;
                    break;
                }

                if (item.SubProject != null && item.SubProject.ProjectItems != null)
                {
                    RetrieveProjectItems(item.SubProject.ProjectItems, worker, e, dic);
                }

                if (item.ProjectItems != null && item.ProjectItems.Count > 0)
                {
                    RetrieveProjectItems(item.ProjectItems, worker, e, dic);
                }

                bool physicalFile = (
                    // a file that belongs to a project
                    item.Kind.CompareTo(Constants.vsProjectItemKindPhysicalFile) == 0 ||
                    // stand-alone file in a solution folder
                    item.Kind.CompareTo(Constants.vsProjectItemKindSolutionItems) == 0
                ) && item.FileCount > 0;

                string itemPath = item.get_FileNames(1);    // VB indexing starts with 1

                if (physicalFile && itemPath != null) 
                {
                    SlnProjectItem slnProjItem;
                    if (dic.TryGetValue(itemPath, out slnProjItem))
                    {   // The file belongs to several projects.
                        slnProjItem.AddProjectName(item.ContainingProject.Name);
                    }
                    else
                    {   // New item
                        int iconIndex = IconIndexFromExtension(
                            Path.GetExtension(item.Name));
                        dic.Add(itemPath, new SlnProjectItem(item, iconIndex));
                    }
                }
            }
        }

        void ClearProjectItems()
        {
            lock (SlnFileFind.ProjectItems)
            {
                _projItems = new ProjectItemsCollection();
            }
        }

        static void FilterProjectItems(ProjectItemsCollection projectItems, string filter)
        {
#if DEBUG
            Stopwatch sw = new Stopwatch();
            sw.Start();
#endif
            projectItems._itemsToShow.Clear();

            if (filter == null || filter.Length == 0)
            {   // just regenerate items-to-show collection
                projectItems.ResetItemsToShow();
            }
            else
            {
                string[] filterTokens = filter.Split(new char[] { ' ' },
                    StringSplitOptions.RemoveEmptyEntries);

                if(filterTokens.Length > 0)
                    FilterProjectItems(projectItems, filterTokens);
            }
#if DEBUG
            sw.Stop();
            Trace.WriteLine(String.Format("Filter {0} out of {1}: {2} msec",
                projectItems._itemsToShow.Count, projectItems._items.Count, sw.ElapsedMilliseconds));
#endif
        }

        static void FilterProjectItems(ProjectItemsCollection projectItems, string[] filterTokens)
        {
            // first non-space character of the first token is '\'
            bool searchPath = (filterTokens[0][0] == '\\');

            for (int itemIndex = 0; itemIndex < projectItems._items.Count; ++itemIndex)
            {
                string filename = searchPath ? projectItems._items[itemIndex].fullPath :
                    projectItems._items[itemIndex].fileName;

                if (IsMatch(filename, filterTokens))
                    projectItems._itemsToShow.Add(itemIndex);
            }
        }

        static bool IsMatch(string filename, string[] filterTokens)
        {
            int startIndex = 0;
            foreach (string token in filterTokens)
            {
                bool negativeMatch = (token[0] == '-');
                string searchValue;

                if (negativeMatch)
                {
                    searchValue = token.Remove(0, 1);
                    if (searchValue.Length == 0)
                        continue;
                }
                else searchValue = token;

                int result = filename.IndexOf(searchValue, startIndex,
                    StringComparison.InvariantCultureIgnoreCase);

                if (negativeMatch)
                {
                    if (result != -1) return false;
                }
                else
                {
                    if (result == -1) return false;

                    startIndex = result + searchValue.Length;
                }
            }

            return true;
        }

        //void ResetItemsToShow()
        //{
        //    _projItems.ResetItemsToShow();
        //}

        static void SortItemsToShow(
            ProjectItemsCollection projectItems,
            SlnProjectItemCompare compare)
        {
#if DEBUG
            Stopwatch sw = new Stopwatch();
            sw.Start();
#endif
            projectItems._itemsToShow.Sort(
                delegate(int x, int y)
                {
                    return compare.Compare(
                        projectItems._items[x], projectItems._items[y]);
                }
            );
#if DEBUG
            sw.Stop();
            Trace.WriteLine(String.Format("Sort {0} items: {1} msec",
                projectItems._itemsToShow.Count, sw.ElapsedMilliseconds));
#endif
        }

        void NotifyCollectionChanged()
        {
            if (CollectionChanged != null)
                CollectionChanged();
        }

        int IconIndexFromExtension(string extension)
        {
            // add '.' if nessesry
            if (!extension.StartsWith(".")) extension = '.' + extension;

            int iconIndex;
            if (_knownExtensions.TryGetValue(extension, out iconIndex))
                return iconIndex;

            SHFILEINFO shfi = new SHFILEINFO();
            uint uFlags = (uint)(
                SHGFI.SHGFI_SYSICONINDEX |
                SHGFI.SHGFI_USEFILEATTRIBUTES |
                SHGFI.SHGFI_SMALLICON
            );

            Shell32.SHGetFileInfo(
                extension,
                0,
                ref shfi,
                (uint)Marshal.SizeOf(shfi),
                uFlags);

            if (shfi.hIcon != IntPtr.Zero)
                User32.DestroyIcon(shfi.hIcon);

            _knownExtensions.Add(extension, shfi.iIcon);

            return shfi.iIcon;
        }

        SlnProjectItemCompare _activeCompare;
        string _pendingFilter;
        Dictionary<string, int> _knownExtensions;
        readonly IntPtr _hSystemImageListSmall;

        BackgroundWorker _bkgWorker;

        class ProjectItemsCollection
        {
            public ProjectItemsCollection()
            {
                _items = new List<SlnProjectItem>();
                _itemsToShow = new List<int>();
            }

            public void Clear()
            {
                _items.Clear();
                _itemsToShow.Clear();
            }

            public void ResetItemsToShow()
            {
                _itemsToShow.Capacity = _items.Count;

                for (int i = 0; i < _items.Count; ++i)
                    _itemsToShow.Add(i);
            }

            public List<SlnProjectItem> _items;
            public List<int> _itemsToShow;
        }

        ProjectItemsCollection _projItems;
    }
}