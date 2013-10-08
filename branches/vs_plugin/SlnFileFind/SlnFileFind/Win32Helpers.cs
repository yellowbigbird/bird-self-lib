using System;
using System.Runtime.InteropServices;
using System.Windows.Forms;

namespace SlnFileFindAddIn
{
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Auto)]
    struct HDITEM
    {
        public uint mask;
        public int cxy;
        public IntPtr pszText;
        public IntPtr hbm;
        public int cchTextMax;
        public int fmt;
        public IntPtr lParam;
    }

    [FlagsAttribute()]
    enum HDItemFlags : uint
    {
        HDI_FORMAT = 0x0004
    }

    [FlagsAttribute()]
    enum HDItemFormat : uint
    {
        HDF_SORTUP   = 0x0400,
        HDF_SORTDOWN = 0x0200
    }

    // ListView messages
    enum SysListView32 : uint
    {
        LVM_FIRST        = 0x1000,
        LVM_SETIMAGELIST = LVM_FIRST + 3,
        LVM_GETHEADER    = LVM_FIRST + 31
    }

    // ListView flags
    enum LVFlags : uint
    {
        LVSIL_NORMAL = 0,
        LVSIL_SMALL  = 1
    }

    // Header messages
    enum SysHeader32 : uint
    {
        HDM_FIRST    = 0x1200,
        HDM_GETITEMW = HDM_FIRST + 11,
        HDM_SETITEMW = HDM_FIRST + 12
    }

    // Edit control messages
    enum Edit : uint
    {
        ECM_FIRST = 0x1500,
        EM_SETCUEBANNER = ECM_FIRST + 1,    // Set the cue banner with the lParm = LPCWSTR
        EM_GETCUEBANNER = ECM_FIRST + 2     // Get the cue banner with the wParm = LPWSTR, lParm = buffer length
    }

    static class User32
    {
        // Declare two overloaded SendMessage functions. The
        // difference is in the last parameter.
        [DllImport("User32.dll")]
        public static extern IntPtr SendMessage(IntPtr hWnd, UInt32 Msg, UInt32 wParam, UInt32 lParam);

        [DllImport("User32.dll", CharSet = CharSet.Unicode)]
        public static extern IntPtr SendMessage(IntPtr hWnd, UInt32 Msg, UInt32 wParam, ref HDITEM lParam);

        [DllImport("User32.dll", CharSet = CharSet.Unicode)]
        public static extern IntPtr SendMessage(IntPtr hWnd, UInt32 Msg, UInt32 wParam, [MarshalAs(UnmanagedType.LPWStr)] string lParam);

        // We need this function to release the unmanaged resource,
        // the unmanaged resource will be copies to a managed one and 
        // it will be returned.
        [DllImport("User32.dll")]
        public static extern Int32 DestroyIcon(IntPtr hIcon);
    }

    static class EditControl
    {
        public static void SetCueBannerText(Control ctrl, string text)
        {
            User32.SendMessage(ctrl.Handle, (uint)Edit.EM_SETCUEBANNER, 0, text);
        }
    }

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Auto)]
    struct SHFILEINFO
    {
        public IntPtr hIcon;
        public int iIcon;
        public uint dwAttributes;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 260)]
        public string szDisplayName;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 80)]
        public string szTypeName;
    }

    [FlagsAttribute()]
    enum SHGFI : uint
    {
        SHGFI_USEFILEATTRIBUTES = 0x000000010,  // use passed dwFileAttribute
        SHGFI_ICON              = 0x000000100,  // get icon
        SHGFI_SYSICONINDEX      = 0x000004000,  // get system icon index

        SHGFI_LARGEICON = 0x000000000,  // get large icon
        SHGFI_SMALLICON = 0x000000001   // get small icon
    }

    enum SHIL : int
    {
        SHIL_SYSSMALL = 3
    }

    static class Shell32
    {
        [DllImport("shell32.dll", CharSet = CharSet.Auto)]
        public static extern IntPtr SHGetFileInfo(
            string pszPath,         // path
            uint dwFileAttributes,  // attributes
            ref SHFILEINFO psfi,    // struct pointer
            uint cbSizeFileInfo,    // size
            uint uFlags             // flags
        );

        [DllImport("shell32.dll")]
        public static extern IntPtr SHGetImageList(
            int iImageList,     // the image type
            ref Guid riid,      // IID_IImageList
            out IntPtr ppvObj   // address of pointer to IImageList
        );

        public static Guid IID_IImageList =
            new Guid("{46EB5926-582E-4017-9FDF-E8998DAA0950}");
    }

    class Win32Window : System.Windows.Forms.IWin32Window
    {
        public Win32Window(IntPtr hWnd)
        {
            _hWnd = hWnd;
        }

        public Win32Window(int hWnd)
        {
            _hWnd = (IntPtr)hWnd;
        }

        #region IWin32Window Members

        public IntPtr Handle
        {
            get { return _hWnd; }
        }

        #endregion

        private IntPtr _hWnd;
    }

    partial class SlnFileFind
    {
        private void UpdateColumnSortMark(SortOrder sortOrder, int columnIndex)
        {
            // Clear previous mark.
            for (int col = 0; col < _lvwFiles.Columns.Count; ++col)
            {
                SetColumnSortMark(SortOrder.None, col);
            }

            // Set new sort mark.
            SetColumnSortMark(sortOrder, columnIndex);
        }

        private void SetColumnSortMark(SortOrder sortOrder, int columnIndex)
        {
            // Get HWND of Header control.
            IntPtr hHeader = User32.SendMessage(
                _lvwFiles.Handle, (UInt32)SysListView32.LVM_GETHEADER, 0, 0);

            // Initialize HDITEM. Retrieve `HDITEM.fmt' field only.
            HDITEM hdi = new HDITEM();
            hdi.mask = (uint)HDItemFlags.HDI_FORMAT;
 
            User32.SendMessage(hHeader, (UInt32)SysHeader32.HDM_GETITEMW,
                (UInt32)columnIndex, ref hdi);

            // Apply sorting arrow indicator flag to `HDITEM.fmt' field.
            hdi.mask = (uint)HDItemFlags.HDI_FORMAT;

            switch (sortOrder)
            {
                case SortOrder.Ascending:
                    hdi.fmt |= (int)HDItemFormat.HDF_SORTUP;
                    break;
                case SortOrder.Descending:
                    hdi.fmt |= (int)HDItemFormat.HDF_SORTDOWN;
                    break;
                case SortOrder.None:
                    hdi.fmt &= ~(int)(HDItemFormat.HDF_SORTUP | HDItemFormat.HDF_SORTDOWN);
                    break;
            }

            // Update column's format.
            User32.SendMessage(hHeader, (UInt32)SysHeader32.HDM_SETITEMW,
                (UInt32)columnIndex, ref hdi);
        }
    }
}