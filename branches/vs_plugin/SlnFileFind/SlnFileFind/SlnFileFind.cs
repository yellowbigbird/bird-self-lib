using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Text;
using System.Windows.Forms;
using EnvDTE;
using EnvDTE80;

namespace SlnFileFindAddIn
{
    public partial class SlnFileFind : UserControl
    {
        public SlnFileFind()
        {
            InitializeComponent();
            InitializeComponent2();
            InitializeComparators();
            SubscribeToEvents();
        }

        ~SlnFileFind()
        {
            UnsubscribeFromEvents();
        }

        public void Activate()
        {
            this.ActiveControl = _txtSearch;
            FileFindWnd.Activate();
        }

        public void Activate(string filterText)
        {
            Activate();

            if (filterText.Length > 0)
            {
                _txtSearch.Text = filterText;
                ApplyFilter();
            }
        }

        public void ResetSearchText()
        {
            _txtSearch.Clear();
        }

        public string CurrentFilter
        {
            get { return _txtSearch.Text; }
        }

        void ApplyFilter()
        {
            // Virtual list-view can operate only on indices,
            // cannot access SelectedItems/CheckedItems collections.
            _lvwFiles.SelectedIndices.Clear();

            SlnFileFind.ProjectItems.ApplyFilter(_txtSearch.Text);

            if (_lvwFiles.VirtualListSize > 0)
            {
                _lvwFiles.AutoResizeColumns(ColumnHeaderAutoResizeStyle.ColumnContent);
            }
        }

        void SubscribeToEvents()
        {
            SlnFileFind.ProjectItems.CollectionChanged +=
                new SlnProjectItems.CollectionChangedHandler(
                    SlnProjectItems_CollectionChanged);

            SlnProjectItems_CollectionChanged();
        }

        void UnsubscribeFromEvents()
        {
            SlnFileFind.ProjectItems.CollectionChanged -=
                new SlnProjectItems.CollectionChangedHandler(
                    SlnProjectItems_CollectionChanged);
        }

        protected override bool ProcessDialogKey(Keys keyData)
        {
            bool handled = false;
            switch (keyData)
            {
                case Keys.Return:
                    OpenSelectedItems();
                    handled = true;
                    break;
                case Keys.Escape:
                    CloseMyself();
                    handled = true;
                    break;
                default:
                    handled = base.ProcessDialogKey(keyData);
                    break;
            }

            return handled;
        }

        void SlnProjectItems_CollectionChanged()
        {
            if (this.InvokeRequired)
            {
                this.BeginInvoke(
                    new MethodInvoker(SlnProjectItems_CollectionChanged));
            }
            else
            {
                SetListSize(SlnFileFind.ProjectItems.Count);
                Invalidate(/*invalidateChildren=*/true);
            }
        }

        private void SetListSize(int size)
        {
            // ListView bug workaround;
            // Google for
            //  NullReferenceException ArgumentOutOfRangeException VirtualListSize
            try
            {
                _lvwFiles.VirtualListSize = 0;
                _lvwFiles.VirtualListSize = size;
            }
            catch (ArgumentOutOfRangeException e)
            {
                Debug.WriteLine("ListView.VirtualListSize: " + e);
            }
            catch (NullReferenceException e)
            {
                Debug.WriteLine("ListView.VirtualListSize: " + e);
            }
        }

        void LvwFiles_RetrieveVirtualItem(object sender, RetrieveVirtualItemEventArgs e)
        {
            lock (SlnFileFind.ProjectItems)
            {
                //Debug.Assert(SlnFileFind.ProjectItems.Count > e.ItemIndex);

                if (SlnFileFind.ProjectItems.Count > e.ItemIndex)
                {
                    SlnProjectItem item = SlnFileFind.ProjectItems[e.ItemIndex];

                    e.Item = new ListViewItem(item.fileName, item.iconIndex);
                    e.Item.SubItems.Add(item.ProjectName);
                    e.Item.SubItems.Add(item.fullPath);
                    e.Item.ToolTipText = item.fullPath + (item.MultipleProjects ?
                        "\r\n_____\r\n" + item.FullProjectName : "");
                }
                else
                {   // If we get here, it means that internal collection size is
                    // out of sync with list-view virtual size.
                    // List-view virtual size is bigger than the number of
                    // actual items we have.
                    // It happens very rarely during shutdown.
                    e.Item = new ListViewItem();
                }
            }
        }

        void LvwFiles_ColumnClick(object sender, ColumnClickEventArgs e)
        {
            SortOrder newSortOrder = GetOppositeSortOrder(e.Column);

            bool sorted = SlnFileFind.ProjectItems.Sort(
                new SlnProjectItemCompare(
                    newSortOrder,
                    _columnSortInfo[e.Column].itemOrder)
            );

            if (sorted)
            {
                SetSortOrder(e.Column, newSortOrder);
                UpdateColumnSortMark(_columnSortInfo[e.Column].sortOder, e.Column);
            }
        }

        void LvwFiles_KeyDown(object sender, KeyEventArgs e)
        {
            switch (e.KeyCode)
            {
                case Keys.Tab:
                    GetNextControl((Control)sender, !e.Shift).Focus();
                    break;
            }
        }

        private void LvwFiles_DoubleClick(object sender, EventArgs e)
        {
            OpenSelectedItems();
        }

        private void LvwFiles_SelectFirstItem()
        {
            if (_lvwFiles.Items.Count > 0)
            {
                if(_lvwFiles.SelectedIndices.Count > 0)
                    _lvwFiles.SelectedIndices.Clear();

                _lvwFiles.Items[0].Focused = true;
                _lvwFiles.Items[0].Selected = true;
            }
        }

        private void TimerSearch_Tick(object sender, EventArgs e)
        {
            _timerSearch.Stop();

            ApplyFilter();
        }

        private void LvwFiles_FocusEnter(object sender, EventArgs e)
        {
            if (_lvwFiles.FocusedItem != null)
                _lvwFiles.FocusedItem.Selected = true;
            else
                LvwFiles_SelectFirstItem();
        }

        void TxtSearch_TextChanged(object sender, EventArgs e)
        {
            _timerSearch.Start();
        }

        void TxtSearch_EnterFocus(object sender, EventArgs e)
        {
            _txtSearch.SelectAll();
        }

        void TxtSearch_KeyDown(object sender, KeyEventArgs e)
        {
            switch (e.KeyCode)
            {
                case Keys.Down:
                    LvwFiles_SelectFirstItem();
                    _lvwFiles.Focus();
                    break;
                case Keys.Enter:
                    if (e.Control && String.Compare(_txtSearch.Text, "settings",
                            StringComparison.InvariantCultureIgnoreCase) == 0)
                    {
                        e.Handled = true;
                        ShowSettings();
                    }
                    break;
            }
        }

        void ShowSettings()
        {
            Settings settings = new Settings();
            settings.SearchDelay = _timerSearch.Interval;
            DialogResult dlgRes = settings.ShowDialog(
                new Win32Window(FileFindWnd.HWnd));

            if (dlgRes == DialogResult.OK)
                _timerSearch.Interval = settings.SearchDelay;

            if (settings.RescanSolution)
                RescanSolution();
        }

        void RescanSolution()
        {
            if (Application == null) return;

            SlnFileFind.ProjectItems.RetrieveSolutionItems(Application);
            SlnFileFind.ProjectItems.ApplyFilter(_txtSearch.Text);
        }

        void OpenSelectedItems()
        {
            List<string> missingFiles = new List<string>();
            Window wnd = null;

            lock (SlnFileFind.ProjectItems)
            {
                // We need to lock SlnFileFind.ProjectItems collection during opening
                // because VS IDE may add/change solution tree items as a result of
                // opened files.
                // Upon change solution tree items and projects will trigger
                // corresponding events and our code will try to recreate
                // SlnFileFind.ProjectItems collection. So, the collection of
                // prject items will die while we're trying to open its items;
                // ListView control quickly gets out of sync with SlnFileFind.ProjectItems
                // and we get a crash. Hence the lock.

                foreach (int i in _lvwFiles.SelectedIndices)
                {
                    if (SlnFileFind.ProjectItems.Count <= i)
                        continue; // see comments in LvwFiles_RetrieveVirtualItem

                    SlnProjectItem slnItem = SlnFileFind.ProjectItems[i];

                    if (System.IO.File.Exists(slnItem.fullPath))
                    {
                        try
                        {
                            wnd = Application.ItemOperations.OpenFile(
                                slnItem.fullPath, Constants.vsViewKindPrimary);
                            wnd.Activate();
                        }
                        catch (System.IO.IOException)
                        {
                            missingFiles.Add(slnItem.fullPath);
                        }
                    }
                    else missingFiles.Add(slnItem.fullPath);
                }
            }

            if (missingFiles.Count > 0)
            {
                StringBuilder sb = new StringBuilder(
                    "Some of the files could not be opened:\r\n\r\n");

                foreach (string filePath in missingFiles)
                    sb.AppendLine(filePath);

                MessageBox.Show(this, sb.ToString(), "Find File in Solution",
                    MessageBoxButtons.OK, MessageBoxIcon.Warning);
            }

            if (wnd != null) // at least one document was opened
                CloseMyself();
        }

        new Control GetNextControl(Control ctl, bool forward)
        {
            Control ctlNext = null;

            // attempt base methos
            ctlNext = base.GetNextControl(ctl, forward);

            // we're at the beginning or end of the controls chain
            if (ctlNext == null)
            {
                int maxTabIndex = 0;
                int maxTabIndexCtl = -1;
                int minTabIndex = this.Controls.Count;
                int minTabIndexCtl = -1;

                for(int index = 0; index < this.Controls.Count; ++index)
                {
                    Control c = this.Controls[index];

                    if (maxTabIndex < c.TabIndex && c.TabStop)
                    {
                        maxTabIndex = c.TabIndex;
                        maxTabIndexCtl = index;
                    }

                    if (minTabIndex > c.TabIndex && c.TabStop)
                    {
                        minTabIndex = c.TabIndex;
                        minTabIndexCtl = index;
                    }
                }

                int nextIndex = (forward ? minTabIndexCtl : maxTabIndexCtl);

                ctlNext = (nextIndex != -1 ? this.Controls[nextIndex] : ctl);
            }

            return ctlNext;
        }

        void InitializeComparators()
        {
            _columnSortInfo = new ColumnSortInfo[3] {
                new ColumnSortInfo(SortOrder.Ascending, SlnProjectItemOrder.ByItemName),
                new ColumnSortInfo(SortOrder.Ascending, SlnProjectItemOrder.ByProjectName),
                new ColumnSortInfo(SortOrder.Ascending, SlnProjectItemOrder.ByItemFullPath)
            };
        }

        void SetSortOrder(int columnIndex, SortOrder newSortOrder)
        {
            _columnSortInfo[columnIndex].sortOder = newSortOrder;
        }

        SortOrder GetOppositeSortOrder(int columnIndex)
        {
            return _columnSortInfo[columnIndex].sortOder == SortOrder.Ascending ?
                    SortOrder.Descending : SortOrder.Ascending;
        }

        void CloseMyself()
        {
            if (FileFindWnd != null && FileFindWnd.IsFloating)
                FileFindWnd.Close(vsSaveChanges.vsSaveChangesNo);
        }

        struct ColumnSortInfo
        {
            public ColumnSortInfo(SortOrder so, SlnProjectItemOrder io)
            {
                sortOder = so;
                itemOrder = io;
            }

            public SortOrder sortOder;
            public SlnProjectItemOrder itemOrder;
        };

        ColumnSortInfo[] _columnSortInfo;
        internal static SlnProjectItems ProjectItems = new SlnProjectItems();

        // The following properties will be set up by code that calls
        // Windows2.CreateToolWindow2. We may need them in the future.
        public DTE2 Application;
        public AddIn AddInInstance;
        public Window FileFindWnd;
    }
}
