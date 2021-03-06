using System;
using System.Drawing;
using System.Reflection;
using System.Resources;
using System.Text.RegularExpressions;
using EnvDTE;
using EnvDTE80;
using Extensibility;
using Microsoft.VisualStudio.CommandBars;
using System.Diagnostics;


namespace SlnFileFindAddIn
{
	/// <summary>The object for implementing an Add-in.</summary>
	/// <seealso class='IDTExtensibility2' />
	public class Connect : IDTExtensibility2, IDTCommandTarget
	{
		/// <summary>Implements the constructor for the Add-in object. Place your initialization code within this method.</summary>
		public Connect()
		{
		}

		/// <summary>Implements the OnConnection method of the IDTExtensibility2 interface. Receives notification that the Add-in is being loaded.</summary>
		/// <param term='application'>Root object of the host application.</param>
		/// <param term='connectMode'>Describes how the Add-in is being loaded.</param>
		/// <param term='addInInst'>Object representing this Add-in.</param>
		/// <seealso class='IDTExtensibility2' />
		public void OnConnection(object application, ext_ConnectMode connectMode, object addInInst, ref Array custom)
		{
			_applicationObject = (DTE2)application;
			_addInInstance = (AddIn)addInInst;

            if (connectMode == ext_ConnectMode.ext_cm_UISetup ||
                !SlnFindFileCommandExists(_applicationObject))
            {
                SetupUI();
            }

            SubscribeToSolutionEvents();

            if (_applicationObject.Solution.IsOpen)
                SolutionEvents_Opened();
		}

        void SubscribeToSolutionEvents()
        {
            UnubscribeFromSolutionEvents();
            
            _slnEvents = _applicationObject.Events.SolutionEvents;
            _slnEvents.Opened += SolutionEvents_Opened;
            _slnEvents.AfterClosing += SolutionEvents_AfterClosing;

            _slnEvents.ProjectAdded += SolutionEvents_ProjectAdded;
            _slnEvents.ProjectRemoved += SolutionEvents_ProjectRemoved;
            _slnEvents.ProjectRenamed += SolutionEvents_ProjectRenamed;
        }

        void SolutionEvents_ProjectRenamed(Project project, string OldName)
        {
            Trace.WriteLine(String.Format("Project renamed: {0} --> {1}", OldName, project.Name));

            OnChangeProject(project);
        }

        void SolutionEvents_ProjectRemoved(Project project)
        {
            Trace.WriteLine(String.Format("Project removed: {0}", project.Name));

            OnChangeProject(project);
        }

        void SolutionEvents_ProjectAdded(Project project)
        {
            Trace.WriteLine(String.Format("Project added: {0}", project.Name));

            OnChangeProject(project);
        }

        void UnubscribeFromSolutionEvents()
        {
            if (_slnEvents != null)
            {
                _slnEvents.Opened -= SolutionEvents_Opened;
                _slnEvents.AfterClosing -= SolutionEvents_AfterClosing;

                _slnEvents.ProjectAdded -= SolutionEvents_ProjectAdded;
                _slnEvents.ProjectRemoved -= SolutionEvents_ProjectRemoved;
                _slnEvents.ProjectRenamed -= SolutionEvents_ProjectRenamed;
            }
        }

        void SolutionEvents_Opened()
        {
            ResetContents();
            SubscribeToProjectsItemsEvents();
        }

        void SubscribeToProjectsItemsEvents()
        {
            UnsubscribeFromProjectsItemsEvents();

            _prjItemsEvents = ((Events2)_applicationObject.Events).ProjectItemsEvents;
            _prjItemsEvents.ItemAdded += ProjectItemsEvents_ItemAdded;
            _prjItemsEvents.ItemRemoved += ProjectItemsEvents_ItemRemoved;
            _prjItemsEvents.ItemRenamed += ProjectItemsEvents_ItemRenamed;
        }

        void UnsubscribeFromProjectsItemsEvents()
        {
            if (_prjItemsEvents != null)
            {
                _prjItemsEvents.ItemAdded -= ProjectItemsEvents_ItemAdded;
                _prjItemsEvents.ItemRemoved -= ProjectItemsEvents_ItemRemoved;
                _prjItemsEvents.ItemRenamed -= ProjectItemsEvents_ItemRenamed;
            }
        }

        void ProjectItemsEvents_ItemRenamed(ProjectItem projectItem, string OldName)
        {
            Trace.WriteLine(String.Format("ProjectItem renamed: {0} --> {1}", OldName, projectItem.Name));

            OnChangeProjectItem(projectItem);
        }

        void ProjectItemsEvents_ItemRemoved(ProjectItem projectItem)
        {
            Trace.WriteLine(String.Format("ProjectItem removed: {0}", projectItem.Name));

            OnChangeProjectItem(projectItem);
        }

        void ProjectItemsEvents_ItemAdded(ProjectItem projectItem)
        {
            Trace.WriteLine(String.Format("ProjectItem added: {0}", projectItem.Name));

            OnChangeProjectItem(projectItem);
        }

        void SolutionEvents_AfterClosing()
        {
            SlnFileFind.ProjectItems.Clear();

            SlnFileFind slnFileFind = GetSlnFileFindControl();

            if (slnFileFind != null)
                slnFileFind.ResetSearchText();
        }

        void SetupUI()
        {
            Commands2 commands = (Commands2)_applicationObject.Commands;
            string toolsMenuName = GetToolMenuName();

            //Place the command on the tools menu.
            //Find the MenuBar command bar, which is the top-level command bar holding all the main menu items:
            CommandBar menuBarCommandBar =
                ((CommandBars)_applicationObject.CommandBars)["MenuBar"];

            //Find the Tools command bar on the MenuBar command bar:
            CommandBarControl toolsControl = menuBarCommandBar.Controls[toolsMenuName];
            CommandBarPopup toolsPopup = (CommandBarPopup)toolsControl;

            // Add-in is visible only if a solution exists in the IDE
            object[] contextGUIDS = new object[] {
                    ContextGuids.vsContextGuidSolutionExists
                };

            const int vsCommandStatusValue =
                (int)vsCommandStatus.vsCommandStatusSupported +
                (int)vsCommandStatus.vsCommandStatusEnabled;

            //This try/catch block can be duplicated if you wish to add multiple commands to be handled by your Add-in,
            //  just make sure you also update the QueryStatus/Exec method to include the new command names.
            try
            {
                //Add a command to the Commands collection:
                Command command = commands.AddNamedCommand2(
                    _addInInstance,
                    "SlnFileFind",
                    "&Find File in Solution",
                    "Executes the command for SlnFileFind",
                    true,
                    46, // blue binoculars
                    ref contextGUIDS,
                    vsCommandStatusValue,
                    (int)vsCommandStyle.vsCommandStylePictAndText,
                    vsCommandControlType.vsCommandControlTypeButton
                );

                //Add default shortcut for the command
                command.Bindings = "Global::Ctrl+Alt+Y";

                //Add a control for the command to the tools menu:
                if ((command != null) && (toolsPopup != null))
                {
                    CommandBarButton cmdButton = (CommandBarButton)command.AddControl(
                        toolsPopup.CommandBar, 1);

                    //InitCommandButtonPicture(cmdButton);
                }
            }
            catch (System.ArgumentException)
            {
                //If we are here, then the exception is probably because a command with that name
                //  already exists. If so there is no need to recreate the command and we can 
                //  safely ignore the exception.
            }
        }

        void OnChangeProjectItem(ProjectItem projectItem)
        {
            bool needReset =
                (projectItem.Kind == Constants.vsProjectItemKindPhysicalFile ||
                 projectItem.Kind == Constants.vsProjectItemKindPhysicalFolder) &&
                projectItem.Kind != Constants.vsProjectItemKindMisc;

            if (needReset)
                ResetContents();
        }

        void OnChangeProject(Project project)
        {
            bool needReset = project.Kind != Constants.vsProjectKindMisc;

            if (needReset)
                ResetContents();
        }

        void ResetContents()
        {
            // We work directly with SlnFileFind.ProjectItems object since
            // SlnFileFind user object may not exist yet at this point.

            SlnFileFind.ProjectItems.RetrieveSolutionItems(_applicationObject);

            SlnFileFind slnFileFind = GetSlnFileFindControl();
            if (slnFileFind != null)
                SlnFileFind.ProjectItems.ApplyFilter(slnFileFind.CurrentFilter);
        }

        string GetToolMenuName()
        {
            string toolsMenuName;
            try
            {
                //If you would like to move the command to a different menu, change the word "Tools" to the 
                //  English version of the menu. This code will take the culture, append on the name of the menu
                //  then add the command to that menu. You can find a list of all the top-level menus in the file
                //  CommandBar.resx.
                ResourceManager resourceManager = new ResourceManager(
                    "SlnFileFindAddIn.CommandBar",
                    Assembly.GetExecutingAssembly()
                );

                System.Globalization.CultureInfo cultureInfo =
                    new System.Globalization.CultureInfo(
                        _applicationObject.LocaleID);

                string resourceName = String.Concat(
                    cultureInfo.TwoLetterISOLanguageName, "Tools");

                toolsMenuName = resourceManager.GetString(resourceName);
            }
            catch
            {
                //We tried to find a localized version of the word Tools, but one was not found.
                //  Default to the en-US word, which may work for the current culture.
                toolsMenuName = "Tools";
            }

            return toolsMenuName;
        }

		/// <summary>Implements the OnDisconnection method of the IDTExtensibility2 interface. Receives notification that the Add-in is being unloaded.</summary>
		/// <param term='disconnectMode'>Describes how the Add-in is being unloaded.</param>
		/// <param term='custom'>Array of parameters that are host application specific.</param>
		/// <seealso class='IDTExtensibility2' />
		public void OnDisconnection(ext_DisconnectMode disconnectMode, ref Array custom)
		{
            if (disconnectMode != ext_DisconnectMode.ext_dm_UISetupComplete)
            {
                UnubscribeFromSolutionEvents();
                UnsubscribeFromProjectsItemsEvents();
            }
		}

		/// <summary>Implements the OnAddInsUpdate method of the IDTExtensibility2 interface. Receives notification when the collection of Add-ins has changed.</summary>
		/// <param term='custom'>Array of parameters that are host application specific.</param>
		/// <seealso class='IDTExtensibility2' />		
		public void OnAddInsUpdate(ref Array custom)
		{
		}

		/// <summary>Implements the OnStartupComplete method of the IDTExtensibility2 interface. Receives notification that the host application has completed loading.</summary>
		/// <param term='custom'>Array of parameters that are host application specific.</param>
		/// <seealso class='IDTExtensibility2' />
		public void OnStartupComplete(ref Array custom)
		{
		}

		/// <summary>Implements the OnBeginShutdown method of the IDTExtensibility2 interface. Receives notification that the host application is being unloaded.</summary>
		/// <param term='custom'>Array of parameters that are host application specific.</param>
		/// <seealso class='IDTExtensibility2' />
		public void OnBeginShutdown(ref Array custom)
		{
            SlnFileFind.ProjectItems.Clear();

            if (_slnFileFindWnd != null)
                _slnFileFindWnd.Close(vsSaveChanges.vsSaveChangesNo);
		}
		
		/// <summary>Implements the QueryStatus method of the IDTCommandTarget interface. This is called when the command's availability is updated</summary>
		/// <param term='commandName'>The name of the command to determine state for.</param>
		/// <param term='neededText'>Text that is needed for the command.</param>
		/// <param term='status'>The state of the command in the user interface.</param>
		/// <param term='commandText'>Text requested by the neededText parameter.</param>
		/// <seealso class='Exec' />
		public void QueryStatus(string commandName, vsCommandStatusTextWanted neededText, ref vsCommandStatus status, ref object commandText)
		{
			if(neededText == vsCommandStatusTextWanted.vsCommandStatusTextWantedNone)
			{
				if(commandName == "SlnFileFindAddIn.Connect.SlnFileFind")
				{
					status = vsCommandStatus.vsCommandStatusSupported |
                             vsCommandStatus.vsCommandStatusEnabled;
				}
			}
		}

		/// <summary>Implements the Exec method of the IDTCommandTarget interface. This is called when the command is invoked.</summary>
		/// <param term='commandName'>The name of the command to execute.</param>
		/// <param term='executeOption'>Describes how the command should be run.</param>
		/// <param term='varIn'>Parameters passed from the caller to the command handler.</param>
		/// <param term='varOut'>Parameters passed from the command handler to the caller.</param>
		/// <param term='handled'>Informs the caller if the command was handled or not.</param>
		/// <seealso class='Exec' />
		public void Exec(string commandName, vsCommandExecOption executeOption, ref object varIn, ref object varOut, ref bool handled)
		{
			handled = false;
			if(executeOption == vsCommandExecOption.vsCommandExecOptionDoDefault)
			{
				if(commandName == "SlnFileFindAddIn.Connect.SlnFileFind")
				{
					handled = true;
                    ShowToolWindow();
				}
			}
		}

        void ShowToolWindow()
        {
            InitToolWindow();

            string initialFilter = GetActiveTextSelection();

            SlnFileFind slnFileFind = (SlnFileFind)_slnFileFindWnd.Object;
            slnFileFind.Activate(initialFilter);
        }

        string GetActiveTextSelection()
        {
            string initialFilter = "";
            TextSelection textSelection = null;

            if (_applicationObject.ActiveWindow != null)
            {   // Init search filter with currrently selected text if there is any.
                textSelection = (TextSelection)_applicationObject.ActiveWindow.Selection;

                if (textSelection == null &&
                    _applicationObject.ActiveWindow.Type == vsWindowType.vsWindowTypeOutput)
                {
                    OutputWindow outWin = (OutputWindow)_applicationObject.ActiveWindow.Object;
                    TextDocument txtDoc = outWin.ActivePane.TextDocument;

                    if (txtDoc != null) textSelection = txtDoc.Selection;
                }
            }

            if (textSelection != null)
                initialFilter = textSelection.Text;

            return initialFilter;
        }

        void InitToolWindow()
        {
            _slnFileFindWnd = FindOurWindow();

            if (_slnFileFindWnd == null)
            {
                EnvDTE80.Windows2 toolWins = (Windows2)_applicationObject.Windows;

                object obUserCtrl = null;

                _slnFileFindWnd = toolWins.CreateToolWindow2(
                    _addInInstance,
                    Assembly.GetExecutingAssembly().Location,  // assembly path
                    "SlnFileFindAddIn.SlnFileFind", // control's class
                    "Find File in Solution",    // caption
                    slnFileFindWndId,   // unique identifier for the new window
                    ref obUserCtrl // User control
                );

                InitUserControl(obUserCtrl);
                InitToolWindowPicture(_slnFileFindWnd);
            }
        }

        int GetMajorAppVersion()
        {
            Regex re = new Regex(@"(\d+)(?:\.\d+)?");
            Match m = re.Match(_applicationObject.Version);
            if (m.Success)
                return int.Parse(m.Groups[1].Value);

            return 0;
        }

        void InitUserControl(object obUserCtrl)
        {
            SlnFileFind sff = obUserCtrl as SlnFileFind;
            if (sff != null)
            {
                sff.Application = _applicationObject;
                sff.AddInInstance = _addInInstance;
                sff.FileFindWnd = _slnFileFindWnd;
            }
        }

        void InitToolWindowPicture(Window toolWnd)
        {
            int majorVer = GetMajorAppVersion();

            stdole.IPictureDisp olePic = GetIPictureFromResource(
                "FindNext", /*makeTransparent=*/(majorVer == 8 ? false : true));
            toolWnd.SetTabPicture(olePic);
        }

        void InitCommandButtonPicture(CommandBarButton cmdButton)
        {
            stdole.IPictureDisp olePic = GetIPictureFromResource(
                "Find", /*makeTransparent=*/false);
            cmdButton.Picture = (stdole.StdPicture)olePic;
        }

        stdole.IPictureDisp GetIPictureFromResource(
            string resPictureName, bool makeTransparent)
        {
            ResourceManager resourceManager = new ResourceManager(
                "SlnFileFindAddIn.CommandBar",
                Assembly.GetExecutingAssembly()
            );

            Bitmap bmpPic = (Bitmap)resourceManager.GetObject(resPictureName);

            if (makeTransparent)
            {
                Color clrBkg = bmpPic.GetPixel(1, 1);
                bmpPic.MakeTransparent(clrBkg);
            }

            return ConvertImage.Convert(bmpPic);
        }

        static bool SlnFindFileCommandExists(DTE2 application)
        {
            Commands2 commands = (Commands2)application.Commands;

            foreach (Command cmd in commands)
            {
                if (cmd.Name == "SlnFileFindAddIn.Connect.SlnFileFind")
                    return true;
            }

            return false;
        }

        Window FindOurWindow()
        {
            foreach (Window wnd in _applicationObject.Windows)
            {
                if (wnd.ObjectKind == slnFileFindWndId)
                    return wnd;
            }

            return null;
        }

        SlnFileFind GetSlnFileFindControl()
        {
            Window wnd = (_slnFileFindWnd != null ? _slnFileFindWnd : FindOurWindow());

            if (wnd != null)
                return (SlnFileFind)wnd.Object;

            return null;
        }

		DTE2 _applicationObject;
		AddIn _addInInstance;
        Window _slnFileFindWnd;

        // Events objects must be class members, so GC won't discard
        // event handlers. (See KB555430)

        SolutionEvents _slnEvents;
        ProjectItemsEvents _prjItemsEvents;

        const string slnFileFindWndId = "{562F1038-659C-46AA-9704-C930596F9BE5}";
	}
}
