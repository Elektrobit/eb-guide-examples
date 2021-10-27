////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Elektrobit Automotive GmbH
// Alle Rechte vorbehalten. All Rights Reserved.
//
// Information contained herein is subject to change without notice.
// Elektrobit retains ownership and all other rights in the software and each
// component thereof.
// Any reproduction of the software or components thereof without the prior
// written permission of Elektrobit is prohibited.
////////////////////////////////////////////////////////////////////////////////

namespace TGAImageLoaderPlugin
{
    using System;
    using System.Globalization;
    using System.IO;
    using System.Windows;
    using System.Windows.Data;
    using System.Windows.Media;

    public class TgaPathToThumbnailConverter : IValueConverter
    {
        private const int DEFAULT_SIZE = 64;

        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {

            var absolutePath = value as string;
            if (null == absolutePath)
            {
                return DependencyProperty.UnsetValue;
            }


            var maxSize = GetIntValue(parameter, DEFAULT_SIZE);

            return File.Exists(absolutePath) ? CreateThumbnail(absolutePath, maxSize) : DependencyProperty.UnsetValue;
        }

        private static ImageSource CreateThumbnail(string absolutePath, int size)
        {
            try
            {
                return CreateImageThumbnail(absolutePath, size);
            }
            catch (Exception)
            {
                return null;
            }
        }

        private static ImageSource CreateImageThumbnail(string absolutePath, int size)
        {
            var tgaReader = new TgaReader(absolutePath);

            return tgaReader.Decode(size);
        }

        private int GetIntValue(object o, int defaultValue)
        {
            var value = defaultValue;
            switch (o)
            {
                case int i:
                    value = i;
                    break;
                case string s:
                    int.TryParse(s, out value);
                    break;
            }

            return value;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return null;
        }
    }
}