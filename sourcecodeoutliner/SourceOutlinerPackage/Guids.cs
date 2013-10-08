/***************************************************************************

Copyright (c) 2006 Microsoft Corporation. All rights reserved.

***************************************************************************/

// Contains a list of Guids for the package.
// This file MUST match guids.h.

using System;

namespace Microsoft.Samples.VisualStudio.PowerToy.SourceOutliner
{
    /// <summary>
    /// Class that provides the Guids for the package.
    /// </summary>
    /// <remarks>
    /// The Guids in this class must match the definitions in the file guids.h.
    /// </remarks>
    static class GuidList
    {
        public const string sourceOutlinerPackageGuidString = "97d413cd-7392-4653-bab3-1430692d6c70";
        public const string sourceOutlinerCmdSetGuidString = "0b6daafa-4d77-48f9-a970-01efba3fe91a";
        public const string toolWindowPersistanceGuidString = "36ac89ff-afe3-4591-be20-e543b6a7c1b1";

        public static readonly Guid guidSourceOutlinerPkg = new Guid(sourceOutlinerPackageGuidString);
        public static readonly Guid guidSourceOutlinerCmdSet = new Guid(sourceOutlinerCmdSetGuidString);
        public static readonly Guid guidToolWindowPersistance = new Guid(toolWindowPersistanceGuidString);
    };
}