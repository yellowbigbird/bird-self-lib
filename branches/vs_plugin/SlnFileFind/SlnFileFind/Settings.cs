using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace SlnFileFindAddIn
{
    public partial class Settings : Form
    {
        public Settings()
        {
            InitializeComponent();
            InitializeComponent2();
        }

        public int SearchDelay
        {
            get { return (int)_searchDelay.Value; }
            set { _searchDelay.Value = value; }
        }

        public bool RescanSolution
        {
            get { return _rescanSolution.Checked; }
        }

        void SearchDelay_KeyDown(object sender, KeyEventArgs e)
        {
            DialogResult dlgRes = DialogResult.None;

            switch (e.KeyCode)
            {
                case Keys.Enter:
                    dlgRes = DialogResult.OK;
                    break;
                case Keys.Escape:
                    dlgRes = DialogResult.Cancel;
                    break;
            }

            if (dlgRes != DialogResult.None)
            {
                e.Handled = true;
                this.DialogResult = dlgRes;
                this.Close();
            }
        }
    }
}
