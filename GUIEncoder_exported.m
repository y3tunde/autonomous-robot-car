classdef GUIEncoder_exported < matlab.apps.AppBase

    % Properties that correspond to app components
    properties (Access = public)
        UIFigure             matlab.ui.Figure
        DirectionGauge       matlab.ui.control.SemicircularGauge
        DirectionGaugeLabel  matlab.ui.control.Label
        CountingLamp_2       matlab.ui.control.Lamp
        CountingLamp_2Label  matlab.ui.control.Label
        UITable              matlab.ui.control.Table
        StopButton           matlab.ui.control.Button
        StartButton          matlab.ui.control.Button
        ConnectButton        matlab.ui.control.Button
        UIAxes_2             matlab.ui.control.UIAxes
        UIAxes               matlab.ui.control.UIAxes
    end

    
    properties (Access = private)
        s
        IdData = []
        LeftData
        RightData
        LeftDistTotalData = []
        RightDistTotalData = []
        DirectionData = []
        TurnTextData
    end
    
    methods (Access = private)
        
        function [Id, LeftCount, RightCount] = parseData(~,data)
        
        % Parse by comma first
        strParseStage1 = split(data,",");
        
        % Now we need to extract the individual components from the parsed data
        Id = str2double(extractAfter(strParseStage1{1}, 'ID:'));
        LeftCount = str2double(extractAfter(strParseStage1{2}, 'LEnc:'));
        RightCount = str2double(extractAfter(strParseStage1{3}, 'REnc:'));
        
        end
    end
    

    % Callbacks that handle component events
    methods (Access = private)

        % Button pushed function: StartButton
        function StartButtonPushed(app, event)
        

            distancePerClick = 2;       %How far encoder travels in on pulse
            distanceScale = 0.001;      %says its in milimeters
            directionThreshold = 0.1;    %minimum difference between left and right encoder before its turning
            terminationID = 100;        %stops at 100 pulses
            
            % =========== Read and process data - We will start from ID2
            while (true)
            
                data=readline(app.s);
            
                % Get (via a function) the ID, Left and Right counts
                [Id, LeftCount, RightCount] = parseData(app,data);
            
                % Check for a termination condition (for example, a specific ID)
                if Id == terminationID 
                    disp('Termination ID received. Exiting loop.');
                    break; % Exit the inner while loop
                end
            
                % Process the counts (for example, display them)
                fprintf('ID: %d, Left Count: %d, Right Count: %d\n', Id, LeftCount, RightCount);
            
                % Store the counts in an array for later plotting
                app.IdData(end+1) = Id;
                app.LeftData = [app.LeftData; LeftCount];
                app.RightData = [app.RightData; RightCount];
            
                % Calculate distances based on counts and scale
                LeftDistance = LeftCount * distancePerClick * distanceScale;
                RightDistance = RightCount * distancePerClick * distanceScale;
            
                % Update total distance data
                app.LeftDistTotalData = [app.LeftDistTotalData; LeftDistance];
                app.RightDistTotalData = [app.RightDistTotalData; RightDistance];
            
                % Update direction data
                app.DirectionData = [app.DirectionData; RightDistance - LeftDistance];
                %disp(DirectionData);
                angle = 0;
                % Check for direction changes based on distance thresholds
                if abs(LeftDistance - RightDistance) > directionThreshold
                    if LeftDistance < RightDistance
                        app.TurnTextData(end + 1) = "LEFT";
                        angle = 0;
                    else
                        app.TurnTextData(end + 1) = "RIGHT";
                        angle = 180;
                    end
                else
                    app.TurnTextData(end + 1) = "STRAIGHT";
                    angle = 90;
                end
                if app.TurnTextData(end) == "LEFT"
                    app.DirectionGauge.Value = 0;
                elseif app.TurnTextData(end) == "STRAIGHT"
                    app.DirectionGauge.Value = 1;
                else
                    app.DirectionGauge.Value = 2;
                end
                % Display the turn direction for the current ID
                disp(['ID: ', num2str(Id), ' - Turn Direction: ', app.TurnTextData(end)]);

                % After updating app.IdData, app.LeftDistTotalData, app.RightDistTotalData

                plot(app.UIAxes, app.IdData, app.LeftDistTotalData);
                plot(app.UIAxes_2, app.IdData, app.RightDistTotalData);
                drawnow limitrate;   % let the UI refres
                
                % Compute R-L difference
                RL = RightDistance - LeftDistance;
                
                % Get direction string

                % Create new row
                newRow = {Id, RL, angle,};
                
                % Append to table
                if isempty(app.UITable.Data)
                    app.UITable.Data = newRow;
                else
                    app.UITable.Data(end+1, :) = newRow;
                end
            end
        end

        % Button pushed function: ConnectButton
        function ConnectButtonPushed(app, event)
             % Nice default values
            Id = 0;
            LeftCount = 0;
            RightCount = 0;
            
            % Initialize arrays to store ID, Left and  Right count values
            app.IdData = [];
            app.LeftData = [];
            app.RightData = []; 
            
            % =================== Open the com port
            app.s=serialport("COM7",9600);

            disp("PORT OPENED");
            app.LeftDistTotalData = [];
            app.RightDistTotalData = [];
            app.DirectionData = [];          % simple indicator (RightDist - LeftDist)
            app.TurnTextData = strings(0);   % "STRAIGHT / LEFT / RIGHT"
            
            % =================== Check com port opened - if not, exit
            if ~isvalid(app.s)
                error('Failed to open COM port. Exiting.');
            end
            
            % ===================  Wait for 'ID:1' to indicate start
            Found = 0;
            while Found == 0
                data=readline(app.s);
                if contains(data, 'ID:1,')
                    Found = 1; % Set Found to 1 to exit the inner loop
                end
            end
            
            disp ("Found the start");
        end

        % Button down function: UIAxes
        function UIAxesButtonDown(app, event)
            x = app.IdData;
            y = app.LeftDistTotalData;
            plot(app.UIAxes, x, y);

        end

        % Button down function: UIAxes_2
        function UIAxes_2ButtonDown(app, event)
            x = app.IdData;
            y = app.RightDistTotalData;
            plot(app.UIAxes, x, y);

        end

        % Callback function
        function UIAxes_3ButtonDown(app, event)

        end

        % Button pushed function: StopButton
        function StopButtonPushed(app, event)
            
        end

        % Display data changed function: UITable
        function UITableDisplayDataChanged(app, event)
            newDisplayData = app.UITable.DisplayData;
            
        end
    end

    % Component initialization
    methods (Access = private)

        % Create UIFigure and components
        function createComponents(app)

            % Create UIFigure and hide until all components are created
            app.UIFigure = uifigure('Visible', 'off');
            app.UIFigure.Position = [100 100 659 481];
            app.UIFigure.Name = 'MATLAB App';

            % Create UIAxes
            app.UIAxes = uiaxes(app.UIFigure);
            title(app.UIAxes, 'LeftEnc')
            xlabel(app.UIAxes, 'ID')
            ylabel(app.UIAxes, 'Distance(m)')
            zlabel(app.UIAxes, 'Z')
            app.UIAxes.ButtonDownFcn = createCallbackFcn(app, @UIAxesButtonDown, true);
            app.UIAxes.Position = [369 273 268 182];

            % Create UIAxes_2
            app.UIAxes_2 = uiaxes(app.UIFigure);
            title(app.UIAxes_2, 'RightEnc')
            xlabel(app.UIAxes_2, 'ID')
            ylabel(app.UIAxes_2, 'Distance(m)')
            zlabel(app.UIAxes_2, 'Z')
            app.UIAxes_2.ButtonDownFcn = createCallbackFcn(app, @UIAxes_2ButtonDown, true);
            app.UIAxes_2.Position = [374 66 263 195];

            % Create ConnectButton
            app.ConnectButton = uibutton(app.UIFigure, 'push');
            app.ConnectButton.ButtonPushedFcn = createCallbackFcn(app, @ConnectButtonPushed, true);
            app.ConnectButton.Position = [18 427 73 28];
            app.ConnectButton.Text = 'Connect';

            % Create StartButton
            app.StartButton = uibutton(app.UIFigure, 'push');
            app.StartButton.ButtonPushedFcn = createCallbackFcn(app, @StartButtonPushed, true);
            app.StartButton.Position = [128 426 79 29];
            app.StartButton.Text = 'Start';

            % Create StopButton
            app.StopButton = uibutton(app.UIFigure, 'push');
            app.StopButton.ButtonPushedFcn = createCallbackFcn(app, @StopButtonPushed, true);
            app.StopButton.Position = [243 425 79 30];
            app.StopButton.Text = 'Stop';

            % Create UITable
            app.UITable = uitable(app.UIFigure);
            app.UITable.ColumnName = {'ID'; 'R-L(meters)'; 'DIRECTION'};
            app.UITable.RowName = {};
            app.UITable.DisplayDataChangedFcn = createCallbackFcn(app, @UITableDisplayDataChanged, true);
            app.UITable.Position = [53 196 269 220];

            % Create CountingLamp_2Label
            app.CountingLamp_2Label = uilabel(app.UIFigure);
            app.CountingLamp_2Label.HorizontalAlignment = 'right';
            app.CountingLamp_2Label.Position = [466 21 53 22];
            app.CountingLamp_2Label.Text = 'Counting';

            % Create CountingLamp_2
            app.CountingLamp_2 = uilamp(app.UIFigure);
            app.CountingLamp_2.Position = [534 20 24 24];

            % Create DirectionGaugeLabel
            app.DirectionGaugeLabel = uilabel(app.UIFigure);
            app.DirectionGaugeLabel.HorizontalAlignment = 'center';
            app.DirectionGaugeLabel.Position = [124 35 88 22];
            app.DirectionGaugeLabel.Text = 'DirectionGauge';

            % Create DirectionGauge
            app.DirectionGauge = uigauge(app.UIFigure, 'semicircular');
            app.DirectionGauge.Limits = [0 2];
            app.DirectionGauge.MajorTicks = [0 1 2];
            app.DirectionGauge.MajorTickLabels = {'LEFT', 'STRAIGHT', 'RIGHT'};
            app.DirectionGauge.Position = [61 72 214 115];
            app.DirectionGauge.Value = 1;

            % Show the figure after all components are created
            app.UIFigure.Visible = 'on';
        end
    end

    % App creation and deletion
    methods (Access = public)

        % Construct app
        function app = GUIEncoder_exported

            % Create UIFigure and components
            createComponents(app)

            % Register the app with App Designer
            registerApp(app, app.UIFigure)

            if nargout == 0
                clear app
            end
        end

        % Code that executes before app deletion
        function delete(app)

            % Delete UIFigure when app is deleted
            delete(app.UIFigure)
        end
    end
end