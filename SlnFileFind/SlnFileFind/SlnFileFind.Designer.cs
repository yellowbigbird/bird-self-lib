namespace SlnFileFindAddIn
{
    partial class SlnFileFind
    {
        /// <summary> 
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary> 
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Component Designer generated code

        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this._txtSearch = new System.Windows.Forms.TextBox();
            this._lvwFiles = new System.Windows.Forms.ListView();
            this._clmnFilename = new System.Windows.Forms.ColumnHeader();
            this._clmnProject = new System.Windows.Forms.ColumnHeader();
            this._clmnPath = new System.Windows.Forms.ColumnHeader();
            this._timerSearch = new System.Windows.Forms.Timer(this.components);
            this.SuspendLayout();
            // 
            // _txtSearch
            // 
            this._txtSearch.Dock = System.Windows.Forms.DockStyle.Top;
            this._txtSearch.Location = new System.Drawing.Point(0, 0);
            this._txtSearch.Name = "_txtSearch";
            this._txtSearch.Size = new System.Drawing.Size(429, 20);
            this._txtSearch.TabIndex = 0;
            this._txtSearch.TextChanged += new System.EventHandler(this.TxtSearch_TextChanged);
            this._txtSearch.KeyDown += new System.Windows.Forms.KeyEventHandler(this.TxtSearch_KeyDown);
            this._txtSearch.Enter += new System.EventHandler(this.TxtSearch_EnterFocus);
            // 
            // _lvwFiles
            // 
            this._lvwFiles.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this._clmnFilename,
            this._clmnProject,
            this._clmnPath});
            this._lvwFiles.Dock = System.Windows.Forms.DockStyle.Fill;
            this._lvwFiles.FullRowSelect = true;
            this._lvwFiles.GridLines = true;
            this._lvwFiles.HideSelection = false;
            this._lvwFiles.Location = new System.Drawing.Point(0, 20);
            this._lvwFiles.Name = "_lvwFiles";
            this._lvwFiles.ShowItemToolTips = true;
            this._lvwFiles.Size = new System.Drawing.Size(429, 445);
            this._lvwFiles.TabIndex = 1;
            this._lvwFiles.UseCompatibleStateImageBehavior = false;
            this._lvwFiles.View = System.Windows.Forms.View.Details;
            this._lvwFiles.VirtualMode = true;
            this._lvwFiles.DoubleClick += new System.EventHandler(this.LvwFiles_DoubleClick);
            this._lvwFiles.Enter += new System.EventHandler(this.LvwFiles_FocusEnter);
            this._lvwFiles.ColumnClick += new System.Windows.Forms.ColumnClickEventHandler(this.LvwFiles_ColumnClick);
            this._lvwFiles.RetrieveVirtualItem += new System.Windows.Forms.RetrieveVirtualItemEventHandler(this.LvwFiles_RetrieveVirtualItem);
            this._lvwFiles.KeyDown += new System.Windows.Forms.KeyEventHandler(this.LvwFiles_KeyDown);
            // 
            // _clmnFilename
            // 
            this._clmnFilename.Text = "File";
            // 
            // _clmnProject
            // 
            this._clmnProject.Text = "Project";
            // 
            // _clmnPath
            // 
            this._clmnPath.Text = "Full Path";
            // 
            // _timerSearch
            // 
            this._timerSearch.Interval = 300;
            this._timerSearch.Tick += new System.EventHandler(this.TimerSearch_Tick);
            // 
            // SlnFileFind
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.AutoSize = true;
            this.Controls.Add(this._lvwFiles);
            this.Controls.Add(this._txtSearch);
            this.Name = "SlnFileFind";
            this.Size = new System.Drawing.Size(429, 465);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        void InitializeComponent2()
        {
            // SlnFileFind
            this.Font = System.Drawing.SystemFonts.MessageBoxFont;

            // Headers
            _clmnFilename.Width = _lvwFiles.Width / 3;
            _clmnPath.AutoResize(
                System.Windows.Forms.ColumnHeaderAutoResizeStyle.HeaderSize);
            UpdateColumnSortMark(System.Windows.Forms.SortOrder.Ascending, 0);

            // Image list
            User32.SendMessage(
                _lvwFiles.Handle,
                (uint)SysListView32.LVM_SETIMAGELIST,
                (uint)LVFlags.LVSIL_SMALL,
                (uint)SlnFileFind.ProjectItems.SmallImageListHandle);

            // Search box cue banner
            string cueBannerText = "enter search strings (e.g., \"sub string -excludeme\") (type \\ to search full path)";
            EditControl.SetCueBannerText(_txtSearch, cueBannerText);
        }

        private System.Windows.Forms.TextBox _txtSearch;
        private System.Windows.Forms.ListView _lvwFiles;
        private System.Windows.Forms.ColumnHeader _clmnFilename;
        private System.Windows.Forms.ColumnHeader _clmnProject;
        private System.Windows.Forms.ColumnHeader _clmnPath;
        private System.Windows.Forms.Timer _timerSearch;
    }
}
