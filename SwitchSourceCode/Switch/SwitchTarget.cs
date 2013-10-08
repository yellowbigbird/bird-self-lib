using System;
using System.Collections.Generic;
using System.Text;

namespace Switch2010
{
    /// <summary>
    /// A Switch Target defines what we switch from, to.
    /// </summary>
    public class SwitchTarget
    {
        /// <summary>
        /// Initializes a new instance of the <see cref="SwitchTarget"/> class.
        /// </summary>
        public SwitchTarget()
        {

        }

        /// <summary>
        /// Initializes a new instance of the <see cref="SwitchTarget"/> class.
        /// </summary>
        /// <param name="from">From.</param>
        /// <param name="to">To.</param>
        public SwitchTarget(string from, string to)
        {
            From = from;
            To = to;
        }

        /// <summary>
        /// Maps the from path to the to path.
        /// </summary>
        /// <param name="path">The path.</param>
        /// <returns>The mapped path</returns>
        public string MapPath(string path)
        {
            //  Replace the path 'from' with the part 'to'.
            if (path.Length < From.Length)
                return null;

            return path.Substring(0, path.Length - From.Length) + To;
        }

        /// <summary>
        /// Gets from.
        /// </summary>
        public string From
        {
            get;
            private set;
        }

        /// <summary>
        /// Gets to.
        /// </summary>
        public string To
        {
            get;
            private set;
        }
    }
}
