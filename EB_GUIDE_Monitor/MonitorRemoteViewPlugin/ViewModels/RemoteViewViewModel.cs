////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Elektrobit Automotive GmbH
// Alle Rechte vorbehalten. All Rights Reserved.
// Information contained herein is subject to change without notice.
// Elektrobit retains ownership and all other rights in the software and each
// component thereof.
// Any reproduction of the software or components thereof without the prior
// written permission of Elektrobit is prohibited.
////////////////////////////////////////////////////////////////////////////////

namespace MonitorRemoteViewPlugin.ViewModels
{
    using System;
    using System.Collections.Generic;
    using System.ComponentModel;
    using System.Linq;
    using System.Windows.Media.Imaging;
    using Elektrobit.Guide.Monitor.Connection;
    using Elektrobit.Guide.Monitor.Messages.Events;
    using Elektrobit.Guide.Monitor.Model.Event;
    using Elektrobit.Guide.Monitor.Model.Value;
    using Elektrobit.Guide.Monitor.Service;
    using Elektrobit.Guide.Monitor.Service.ScreenshotService;
    using Elektrobit.Guide.Monitor.Ui.Dialog;
    using Elektrobit.Guide.Monitor.Ui.Docking;
    using Elektrobit.Guide.Monitor.Utilities;
    using Elektrobit.Guide.Scheduler;
    using Elektrobit.Guide.Ui.Mvvm;

    using MonitorRemoteViewPlugin.Utilities.ContinuousScreenshot;
    using MonitorRemoteViewPlugin.Utilities.FrameDelayConverter;
    using MonitorRemoteViewPlugin.Utilities.ImageClick;
    using Prism.Commands;

    public class RemoteViewViewModel : ViewModel, IDockableComponentViewModel, IMouseClickHandler
    {
        private const int DEFAULT_FINGER_ID = 0;
        private const int MESSAGE_ID = 3;
        private const string DIALOG_WINDOW_TITLE = "Save Image As";
        private const string DEFAULT_SCREENSHOT_NAME = "Screenshot";
        private const string FILETYPE_FILTER = " PNG image file (*.png) | *.png";

        private readonly IScreenshotService _screenshotService;
        private readonly IScreenshotSavingUtility _screenshotSavingUtility;
        private readonly ILoggerService _loggerService;
        private readonly IConnectionService _connectionService;

        private readonly ITaskSchedulerProvider _schedulerProvider;

        private readonly IEventService _eventService;
        private readonly IContinuousScreenshotController _continuousScreenshotController;
        private readonly IFrameDelayConverter _frameDelayConverter;
        private readonly IMonitorDialog _monitorDialog;

        private BitmapSource _screenshotSource;
        private uint _framesPerSecond;

        public const int TOUCH_PRESS = 0;
        public const int TOUCH_RELEASE = 2;

        public DelegateCommand Refresh { get; private set; }
        public DelegateCommand Save { get; private set; }
        public DelegateCommand AutoRefresh { get; private set; }

        public BitmapSource ScreenshotSource
        {
            get => _screenshotSource;

            set
            {
                SetProperty(ref _screenshotSource, value);
                Save.RaiseCanExecuteChanged();
            }
        }

        public uint FramesPerSecond
        {
            get => _framesPerSecond;
            set
            {
                _framesPerSecond = value;
                var delay = _frameDelayConverter == null ? 1000 : _frameDelayConverter.Convert(_framesPerSecond);
                if (_continuousScreenshotController != null)
                {
                    _continuousScreenshotController.Delay = delay;
                }
            }
        }

        public uint SelectedScene { get; set; }

        public RemoteViewViewModel(
            IScreenshotService screenshotService,
            IScreenshotSavingUtility screenshotSavingUtility,
            ILoggerService loggerService,
            IConnectionService connectionService,
            ITaskSchedulerProvider schedulerProvider,
            IEventService eventService,
            IContinuousScreenshotController continuousScreenshotController,
            IFrameDelayConverter frameDelayConverter,
            IMonitorDialog monitorDialog)
        {
            _screenshotService = screenshotService;
            _screenshotSavingUtility = screenshotSavingUtility;
            _loggerService = loggerService;
            _connectionService = connectionService;
            _schedulerProvider = schedulerProvider;
            _eventService = eventService;
            _continuousScreenshotController = continuousScreenshotController;
            _frameDelayConverter = frameDelayConverter;
            _monitorDialog = monitorDialog;

            if (_continuousScreenshotController != null)
            {
                _continuousScreenshotController.PropertyChanged += ScreenshotChanged;
            }

            InitializeCommands();
            InitializeScenes();
            InitializeFramesPerSecond();

            if (_connectionService.IsConnected)
            {
                SubscribeTouchEvents();
            }
        }

        private void OnMessageReceived(object sender, MessageReceivedEventArgs e)
        {
            if (e.Message is FireEvent message && EventGroups.TouchEvents.Contains(message.GroupId))
            {
                _loggerService.LogMessage(this, message.GroupId.ToString(), Severity.Info);
            }
        }

        private void OnConnected(object sender, EventArgs e)
        {
            SubscribeTouchEvents();
        }

        private void SubscribeTouchEvents()
        {
            var message = new SubscribeEventGroups() { Subscribe = true, GroupIds = EventGroups.TouchEvents.ToList() };
            _connectionService.SendAsync(message);
        }

        private void InitializeScenes()
        {
            SelectedScene = 0;
        }

        private void InitializeCommands()
        {
            Save = new DelegateCommand(OnSave, CanSave);
            Refresh = new DelegateCommand(OnRefresh, CanRefresh);
            AutoRefresh = new DelegateCommand(OnAutoRefresh, CanAutoRefresh);

            _connectionService.Connected += (sender, args) => Refresh.RaiseCanExecuteChanged();
            _connectionService.Disconnected += (sender, args) => Refresh.RaiseCanExecuteChanged();

            _connectionService.Connected += (sender, args) => AutoRefresh.RaiseCanExecuteChanged();
            _connectionService.Disconnected += (sender, args) => AutoRefresh.RaiseCanExecuteChanged();

            _connectionService.Connected += OnConnected;
            _connectionService.MessageReceived += OnMessageReceived;
        }

        private void InitializeFramesPerSecond()
        {
            FramesPerSecond = 1;
        }

        public void Dispose()
        {
            _continuousScreenshotController.PropertyChanged -= ScreenshotChanged;
            _continuousScreenshotController.StopAsync(SelectedScene);
        }

        private void ScreenshotChanged(object sender, PropertyChangedEventArgs e)
        {
            if (sender != _continuousScreenshotController)
            {
                return;
            }

            if (e.PropertyName != "Screenshot")
            {
                return;
            }

            ScreenshotSource = _continuousScreenshotController?.Screenshot?.BitmapSource;
        }

        private async void OnRefresh()
        {
            var screenshot = await _screenshotService.GetScreenshotAsync(SelectedScene);
            ScreenshotSource = screenshot.BitmapSource;
        }

        private bool CanRefresh()
        {
            return _connectionService.IsConnected;
        }

        private async void OnSave()
        {
            try
            {
                await _schedulerProvider.MainScheduler.InvokeAsync(() => _continuousScreenshotController.ToggleAsync(SelectedScene));

                var fileSelectionResult = _monitorDialog.SaveFileDialog(DIALOG_WINDOW_TITLE, DEFAULT_SCREENSHOT_NAME, FILETYPE_FILTER);
                var path = fileSelectionResult.FileName;

                if (!fileSelectionResult.Success)
                {
                    return;
                }

                _screenshotSavingUtility.SaveScreenshotFromBitmapSource(ScreenshotSource, path);

                _loggerService.LogMessage(this, "Screenshot saved.", Severity.Info);
            }
            catch (Exception ex)
            {
                _loggerService.LogError(this, $"Screenshot saving failed. {ex.Message}");
            }
        }

        private bool CanSave()
        {
            return ScreenshotSource != null;
        }

        private async void OnAutoRefresh()
        {
            await _schedulerProvider.MainScheduler.InvokeAsync(() => _continuousScreenshotController.ToggleAsync(SelectedScene));
        }

        private bool CanAutoRefresh()
        {
            return _connectionService.IsConnected;
        }

        public void MouseClick(double x, double y)
        {
            var scene = new ModelInteger((int)SelectedScene);
            var touchType = new ModelInteger(TOUCH_PRESS);
            var fingerId = new ModelInteger(DEFAULT_FINGER_ID);

            if (ScreenshotSource == null)
            {
                return;
            }

            var imageXCoordinate = new ModelInteger((int)(x * ScreenshotSource.Width));
            var imageYCoordinate = new ModelInteger((int)(y * ScreenshotSource.Height));

            if (!_continuousScreenshotController.IsRunning)
            {
                return;
            }

            if (x > 1 || y > 1 || x < 0 || y < 0)
            {
                return;
            }

            var itemsList = new List<ModelValue> { scene, touchType, imageXCoordinate, imageYCoordinate, fingerId };
            var touchEvent = new Event(MESSAGE_ID, 11);

            _eventService.FireEvent(touchEvent, itemsList);
            touchType.Value = TOUCH_RELEASE;
            _eventService.FireEvent(touchEvent, itemsList);
        }
    }
}