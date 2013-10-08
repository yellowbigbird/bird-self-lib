using System;
using System.Collections.Generic;
using System.Text;
using EnvDTE;
using System.IO;
using EnvDTE80;

namespace Switch2010
{
    /// <summary>
    /// The Switcher switches between related files.
    /// </summary>
    public class Switcher
    {
        /// <summary>
        /// Prevents a default instance of the <see cref="Switcher"/> class from being created.
        /// </summary>
        private Switcher()
        {
            //  Create the switch targets.
            switchTargets.Add(new SwitchTarget("c", "h"));
            switchTargets.Add(new SwitchTarget("cpp", "h"));
            switchTargets.Add(new SwitchTarget("h", "c"));
            switchTargets.Add(new SwitchTarget("h", "cpp"));
            switchTargets.Add(new SwitchTarget("xaml", "xaml.cs"));
            switchTargets.Add(new SwitchTarget("xaml.cs", "xaml"));
        }

        /// <summary>
        /// The singleton instance.
        /// </summary>
        private static Switcher instance = null;

        /// <summary>
        /// Gets the instance.
        /// </summary>
        public static Switcher Instance
        {
            get
            {
                if (instance == null)
                    instance = new Switcher();
                return instance;
            }
        }

        /// <summary>
        /// The switch targets.
        /// </summary>
        private List<SwitchTarget> switchTargets = new List<SwitchTarget>();

        /// <summary>
        /// Switches the specified active document, by returning
        /// the appropriate target document.
        /// </summary>
        /// <param name="application">The application.</param>
        /// <param name="activeDocument">The active document.</param>
        public void Switch(DTE2 application, Document activeDocument)
        {
            //  Does the active document have a designer?
            if (DoesDocumentHaveDesigner(application, activeDocument.FullName))
            {
                //  It does, so just switch between the designer and code view.
                TryToggleCodeDesignView(activeDocument);
            }
            
            //  Go through each switch target - if we have
            //  one for this file, we can attempt to switch.
            List<SwitchTarget> targets = new List<SwitchTarget>();
            foreach (var target in switchTargets)
                if (activeDocument.FullName.EndsWith(target.From))
                    targets.Add(target);

            //  Go through each potential target, try and open
            //  the document. If it opens, we're done.
            foreach (var target in targets)
                if(TryOpenDocument(application, target.MapPath(activeDocument.FullName)))
                    break;
        }

        /// <summary>
        /// Tries to the toggle between the code and design view.
        /// </summary>
        /// <param name="activeDocument">The active document.</param>
        private void TryToggleCodeDesignView(Document activeDocument)
        {
            //  If we're showing the designer, show the code view.
            if (activeDocument.ActiveWindow.Caption.Contains("[Design]"))
                activeDocument.ProjectItem.Open(vsViewKindCode).Activate();
            else
                activeDocument.ProjectItem.Open(vsViewKindDesigner).Activate();
        }

        /// <summary>
        /// Determines whether a document has a designer.
        /// </summary>
        /// <param name="application">The application.</param>
        /// <param name="path">The path of the document to check.</param>
        /// <returns>True if the document has a designer.</returns>
        private bool DoesDocumentHaveDesigner(DTE2 application, string path)
        {
            //  Create the designer path.
            string designerPath = Path.Combine(Path.GetDirectoryName(path), Path.GetFileNameWithoutExtension(path)) + ".designer.cs";

            //  Is there a designer document?
            return File.Exists(designerPath);
        }

        private bool TryOpenDocument(DTE2 application, string path)
        {
            //  Go through each document in the solution.
            foreach(Document document in application.Documents)
            {
                if(string.Compare(document.FullName, path) == 0)
                {
                    //  The document is open, we just need to activate it.
                    if (document.Windows.Count > 0)
                    {
                        document.Activate();
                        return true;
                    }
                }
            }

            //  The document isn't open - does it exist?
            if (File.Exists(path))
            {
                try
                {
                    application.Documents.Open(path, "Text", false);
                    return true;
                }
                catch
                {
                }
            }

            //  We couldn't open the document.
            return false;
        }

        internal const string vsViewKindCode = "{7651A701-06E5-11D1-8EBD-00A0C90F26EA}";
        internal const string vsViewKindDebugging = "{7651A700-06E5-11D1-8EBD-00A0C90F26EA}";
        internal const string vsViewKindDesigner = "{7651A702-06E5-11D1-8EBD-00A0C90F26EA}";
    }
}
