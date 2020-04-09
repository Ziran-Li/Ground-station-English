// --------------------------------------------------------------------------------------------------------------------
// <copyright file="MainViewModel.cs" company="Helix 3D Toolkit">
//   http://helixtoolkit.codeplex.com, license: MIT
// </copyright>
// --------------------------------------------------------------------------------------------------------------------


using System;
using System.Collections.Generic;
using System.Diagnostics.CodeAnalysis;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Input;
using System.Windows.Media.Media3D;
using System.Windows.Threading;

using HelixToolkit.Wpf;

namespace XMonitor_GUI.Window
{
    class MainViewModel
    {
        private readonly Dispatcher dispatcher;
        private Model3D currentModel;
        private readonly IHelixViewport3D viewport;

        public MainViewModel(HelixViewport3D viewport)
        {
            if (viewport == null)
            {
                throw new ArgumentNullException("viewport");
            }
            this.viewport = viewport;
            this.dispatcher = Dispatcher.CurrentDispatcher;
            var mi = new ModelImporter();
            currentModel = mi.Load(@"Resources\06L.STL", this.dispatcher);
            this.viewport.ZoomExtents(1000000);

        }

        
        public Model3D CurrentModel
        {
            get
            {
                return this.currentModel;
            }

            set
            {
                this.currentModel = value;
            }
        }
        private void ViewZoomExtents()
        {
            this.viewport.ZoomExtents(5000);
        }
    }
}
