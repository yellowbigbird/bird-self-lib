namespace SlnFileFindAddIn
{
    partial class Settings
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

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.Windows.Forms.Label label2;
            System.Windows.Forms.GroupBox groupBox1;
            System.Windows.Forms.Label label1;
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Settings));
            this._searchDelay = new System.Windows.Forms.NumericUpDown();
            this._rescanSolution = new System.Windows.Forms.CheckBox();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            label2 = new System.Windows.Forms.Label();
            groupBox1 = new System.Windows.Forms.GroupBox();
            label1 = new System.Windows.Forms.Label();
            groupBox1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this._searchDelay)).BeginInit();
            this.groupBox2.SuspendLayout();
            this.SuspendLayout();
            // 
            // label2
            // 
            label2.AutoSize = true;
            label2.Location = new System.Drawing.Point(158, 23);
            label2.Name = "label2";
            label2.Size = new System.Drawing.Size(32, 13);
            label2.TabIndex = 2;
            label2.Text = "msec";
            // 
            // groupBox1
            // 
            groupBox1.Controls.Add(label1);
            groupBox1.Controls.Add(label2);
            groupBox1.Controls.Add(this._searchDelay);
            groupBox1.Location = new System.Drawing.Point(8, 12);
            groupBox1.Name = "groupBox1";
            groupBox1.Size = new System.Drawing.Size(245, 56);
            groupBox1.TabIndex = 0;
            groupBox1.TabStop = false;
            groupBox1.Text = "Search";
            // 
            // label1
            // 
            label1.AutoSize = true;
            label1.Location = new System.Drawing.Point(6, 23);
            label1.Name = "label1";
            label1.Size = new System.Drawing.Size(72, 13);
            label1.TabIndex = 0;
            label1.Text = "&Search delay:";
            // 
            // _searchDelay
            // 
            this._searchDelay.Increment = new decimal(new int[] {
            100,
            0,
            0,
            0});
            this._searchDelay.Location = new System.Drawing.Point(86, 19);
            this._searchDelay.Maximum = new decimal(new int[] {
            1000,
            0,
            0,
            0});
            this._searchDelay.Name = "_searchDelay";
            this._searchDelay.Size = new System.Drawing.Size(72, 20);
            this._searchDelay.TabIndex = 1;
            this._searchDelay.KeyDown += new System.Windows.Forms.KeyEventHandler(this.SearchDelay_KeyDown);
            // 
            // _rescanSolution
            // 
            this._rescanSolution.AutoSize = true;
            this._rescanSolution.Location = new System.Drawing.Point(9, 19);
            this._rescanSolution.Name = "_rescanSolution";
            this._rescanSolution.Size = new System.Drawing.Size(225, 17);
            this._rescanSolution.TabIndex = 0;
            this._rescanSolution.Text = "&Rescan solution when this dialog is closed";
            this._rescanSolution.UseVisualStyleBackColor = true;
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this._rescanSolution);
            this.groupBox2.Location = new System.Drawing.Point(8, 79);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(245, 58);
            this.groupBox2.TabIndex = 1;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Contents";
            // 
            // Settings
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(262, 153);
            this.Controls.Add(groupBox1);
            this.Controls.Add(this.groupBox2);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "Settings";
            this.Text = "Settings";
            groupBox1.ResumeLayout(false);
            groupBox1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this._searchDelay)).EndInit();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        void InitializeComponent2()
        {
            // Settings
            this.Font = System.Drawing.SystemFonts.MessageBoxFont;
        }

        private System.Windows.Forms.NumericUpDown _searchDelay;
        private System.Windows.Forms.CheckBox _rescanSolution;
        private System.Windows.Forms.GroupBox groupBox2;
    }
}