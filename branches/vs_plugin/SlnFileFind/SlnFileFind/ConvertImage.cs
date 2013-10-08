
namespace SlnFileFindAddIn
{
    sealed class ConvertImage : System.Windows.Forms.AxHost
    {
        private ConvertImage()
            : base(null)
        {
        }

        public static stdole.IPictureDisp Convert
            (System.Drawing.Image image)
        {
            return (stdole.IPictureDisp)System.Windows.Forms.
                AxHost.GetIPictureDispFromPicture(image);
        }

        public static System.Drawing.Image Convert(stdole.IPictureDisp image)
        {
            if (image.Type == 1)
            {
                System.IntPtr hPal = (System.IntPtr)image.hPal;
                return System.Drawing.Image.FromHbitmap(
                    (System.IntPtr)image.Handle, hPal);
            }

            return null;
        }
    }
}