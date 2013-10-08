/***************************************************************************

Copyright (c) 2006 Microsoft Corporation. All rights reserved.

***************************************************************************/

using EnvDTE;
using Microsoft.VisualStudio;
using Microsoft.VisualStudio.OLE.Interop;
using Microsoft.VisualStudio.Shell;
using Microsoft.VisualStudio.Shell.Interop;
using System;
using System.ComponentModel.Design;
using System.Diagnostics;
using System.Globalization;
using System.Runtime.InteropServices;

namespace Microsoft.Samples.VisualStudio.PowerToy.SourceOutliner
{
    /// <summary>
    /// Class that implements the package exposed by this assembly.
    /// </summary>
    ///<remarks>
    /// The minimum requirement for a class to be considered a valid package for Visual Studio
    /// is to implement the IVsPackage interface and register itself with the shell.
    /// This package uses the helper classes defined inside the Managed Package Framework (MPF)
    /// to do it: it derives from the Package class that provides the implementation of the 
    /// IVsPackage interface and uses the registration attributes defined in the framework to 
    /// register itself and its components with the shell.
    /// </remarks>

    // This attribute tells the registration utility (regpkg.exe) that this class needs
    // to be registered as package.
    [PackageRegistration(UseManagedResourcesOnly = true)]

    // A Visual Studio component can be registered under different Registry roots; for instance
    // when you debug your package, you want to register it in the experimental hive. This
    // attribute specifies the Registry root to use if no one is provided to regpkg.exe with
    // the /root switch.
    [DefaultRegistryRoot("Software\\Microsoft\\VisualStudio\\9.0")]

    // This attribute is used to register the information needed to show the this package
    // in the Help/About dialog of Visual Studio.
    [InstalledProductRegistration(false, "#100", "#102", "1.0", IconResourceID = 400)]

    // In order be loaded inside Visual Studio in a computer that does not have the 
    // VS SDK installed, a package needs to have a valid load key (it can be requested at 
    // http://msdn.microsoft.com/vstudio/extend/). This attributes tells the shell that this 
    // package has a load key embedded in its resources.
    [ProvideLoadKey(PackageConstants.VisualStudioEdition, PackageConstants.VersionString, PackageConstants.ProductName, PackageConstants.CompanyName, PackageConstants.PLKResourceID)]

    // This attribute is needed to let the shell know that this package exposes some menus.
    [ProvideMenuResource(1000, 1)]

    // This attribute registers a tool window exposed by this package.
    // It will initially be docked at the toolbox window location.
    [ProvideToolWindow(typeof(SourceOutlineToolWindow), Width = 300, Height = 450, Style = VsDockStyle.Tabbed
      )]

    [CLSCompliant(false)]

    [Guid(GuidList.sourceOutlinerPackageGuidString)]
    public sealed class SourceOutliner : Package
    {
        private const int _bitmapResourceID = 300;
        private EnvDTE.DTE _dte;
        private IOleComponentManager _componentManager = null;
        private uint _componentID;
        private SourceOutlineToolWindow _window = null;

        /// <summary>
        /// Initializes a new instance of the SourceOutliner class.
        ///</summary>
        /// <remarks>
        /// Inside this method you can place any initialization code that does not require 
        /// any Visual Studio service, because at this point the package object is created but 
        /// not sited yet inside Visual Studio environment. The place to do all the other 
        /// initialization is the Initialize method.
        /// </remarks>
        public SourceOutliner()
        {
            DisplayMessage(Resources.StatusPrefix, string.Format(CultureInfo.CurrentCulture, "Entering constructor for: {0}", this.ToString()));
        }

        /// <summary>
        /// Called when the user clicks the menu item that shows the tool window.
        /// </summary>
        /// <param name="sender">The source object for this event.</param>
        /// <param name="e">An EventsArgs that contains the event data.</param>
        /// <remarks>
        /// See the Initialize method to see how the menu item is associated with 
        /// this function using the OleMenuCommandService service and the MenuCommand class.
        /// </remarks>
        private void ShowToolWindow(object sender, EventArgs e)
        {
            try
            {
                IVsWindowFrame windowFrame = (IVsWindowFrame)_window.Frame;
                ErrorHandler.ThrowOnFailure(windowFrame.Show());
            }
            catch (Exception ex)
            {
                DisplayMessage(Resources.ErrorPrefix, "ShowToolWindow exception: " + ex.ToString());
            }
        }

        /// <summary>
        /// Returns a resource string by name.
        /// </summary>
        /// <param name="resourceName">The name of the resource to lookup.</param>
        /// <returns>A display string.</returns>
        public static string GetResourceString(string resourceName)
        {
            string resourceValue;
            IVsResourceManager resourceManager = (IVsResourceManager)GetGlobalService(typeof(SVsResourceManager));
            Guid packageGuid = typeof(SourceOutliner).GUID;
            int hr = resourceManager.LoadResourceString(ref packageGuid, -1, resourceName, out resourceValue);
            Microsoft.VisualStudio.ErrorHandler.ThrowOnFailure(hr);
            return resourceValue;
        }

        /// <summary>
        /// Returns a resource string by ID.
        /// </summary>
        /// <param name="resourceId">The ID of the resource to lookup.</param>
        /// <returns>A display string.</returns>
        public static string GetResourceString(int resourceId)
        {
            return GetResourceString(string.Format("#{0}", resourceId));
        }

        /// <summary>
        /// Gets the component manager for the package.
        /// </summary>
        /// <returns>An IOleComponentManager object.</returns>
        public IOleComponentManager ComponentManager
        {
            get 
            {
                return _componentManager; 
            }
        }

        /// <summary>
        /// Changes the cursor to the hourglass cursor. 
        /// </summary>
        /// <returns>A return code or S_OK.</returns>
        public int SetWaitCursor()
        {
            int hr = VSConstants.S_OK;

            IVsUIShell VsUiShell = GetService(typeof(SVsUIShell)) as IVsUIShell;
            if (VsUiShell != null)
            {
                // There is no check for return code because 
                // any failure of this call is ignored.
                hr = VsUiShell.SetWaitCursor();
            }

            return hr;
        }

        /////////////////////////////////////////////////////////////////////////////
        // Overridden Package Implementation
        #region Package Members

        /// <summary>
        /// Initialization of the package; this method is called right after the package is sited, so this is the place
        /// where you can put any initialization code that relies on services provided by Visual Studio.
        /// </summary>
        protected override void Initialize()
        {
            DisplayMessage(Resources.StatusPrefix, string.Format(CultureInfo.CurrentCulture, "Entering Initialize() of: {0}", this.ToString()));
            base.Initialize();

            // Add command handlers for the menu (commands must exist in the .ctc file)
            OleMenuCommandService mcs = GetService(typeof(IMenuCommandService)) as OleMenuCommandService;
            if (null != mcs)
            {
                // Create the command for the tool window in the Other Windows menu.
                CommandID toolwndCommandID = new CommandID(GuidList.guidSourceOutlinerCmdSet, (int)PkgCmdIDList.cmdidSourceOutliner1);
                MenuCommand menuToolWin = new MenuCommand(new EventHandler(ShowToolWindow), toolwndCommandID);
                mcs.AddCommand(menuToolWin);
            }

            _dte = GetService(typeof(EnvDTE._DTE)) as EnvDTE.DTE;
            if (_dte == null)
            {
                throw new NullReferenceException("DTE is null");
            }

            _window = (SourceOutlineToolWindow)this.FindToolWindow(typeof(SourceOutlineToolWindow), 0, true);
            _window.Package = this;

            _componentManager = (IOleComponentManager)GetService(typeof(SOleComponentManager));
            if (_componentID == 0)
            {
                OLECRINFO[] crinfo = new OLECRINFO[1];
                crinfo[0].cbSize = (uint)Marshal.SizeOf(typeof(OLECRINFO));
                crinfo[0].grfcrf = (uint)_OLECRF.olecrfNeedIdleTime | (uint)_OLECRF.olecrfNeedPeriodicIdleTime 
                                    | (uint)_OLECRF.olecrfNeedAllActiveNotifs | (uint)_OLECRF.olecrfNeedSpecActiveNotifs;
                crinfo[0].grfcadvf = (uint)_OLECADVF.olecadvfModal | (uint)_OLECADVF.olecadvfRedrawOff | (uint)_OLECADVF.olecadvfWarningsOff;
                crinfo[0].uIdleTimeInterval = 100;

                int hr = _componentManager.FRegisterComponent(_window, crinfo, out this._componentID);
                if (!ErrorHandler.Succeeded(hr))
                {
                    DisplayMessage(Resources.ErrorPrefix, "Initialize->IOleComponent registration failed");
                }
            }

            // Initialize the DTE and the code outline file manager, and hook up events.
            InitializeToolWindow();
        }

        /// <summary>
        /// Initializes the DTE and the code outline file manager, and hooks up events.
        /// </summary>
        private void InitializeToolWindow()
        {
            _window.InitializeDTE(_dte);
            _window.AddWindowEvents();
            _window.AddSolutionEvents();
        }

        /// <summary>
        /// Cleans up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            try
            {
                if (disposing)
                {
                    DisplayMessage(Resources.StatusPrefix, string.Format(CultureInfo.CurrentCulture, "Entering Dispose() of: {0}", this.ToString()));

                    // Unregister the idle loop handler.
                    _componentManager.FRevokeComponent(_componentID);
                }
            }
            finally
            {
                base.Dispose(disposing);
            }
        }

        #endregion Package Members

        /// <summary>
        /// Displays the specified message string.
        /// </summary>
        /// <param name="prefix">An optional message prefix, such as Status: or Error:.</param>
        /// <param name="message">The message to write.</param>
        public static void DisplayMessage(string prefix, string message)
        {
            // Messages are current shown to the trace, not the user.
            // Change this to a MessageBox to display in the UI.
            string output = message.Trim();
            if (prefix.Length > 0)
            {
                output = prefix.Trim() + " " + output + Environment.NewLine;
            }
            Trace.WriteLine(output);
        }
    }
}