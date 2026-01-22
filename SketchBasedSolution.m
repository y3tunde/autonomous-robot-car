clear;

distancePerClick = 2;       %How far encoder travels in on pulse
distanceScale = 0.001;      %says its in milimeters
directionThreshold = 0.1;    %minimum difference between left and right encoder before its turning
terminationID = 100;        %stops at 100 pulses

% Nice default values
Id = 0;
LeftCount = 0;
RightCount = 0;

% Initialize arrays to store ID, Left and  Right count values
IdData = [];
LeftData = [];
RightData = []; 

% =================== Open the com port
s=serialport("COM7",9600);
disp("PORT OPENED");
LeftDistTotalData = [];
RightDistTotalData = [];
DirectionData = [];          % simple indicator (RightDist - LeftDist)
TurnTextData = strings(0);   % "STRAIGHT / LEFT / RIGHT"

% =================== Check com port opened - if not, exit
if ~isvalid(s)
    error('Failed to open COM port. Exiting.');
end

% ===================  Wait for 'ID:1' to indicate start
Found = 0;
while Found == 0
    data=readline(s);
    if contains(data, 'ID:1,')
        Found = 1; % Set Found to 1 to exit the inner loop
    end
end

disp ("Found the start");


%% =================== Set up plots (3 plots: Left, Right, Direction)
tiledlayout(3,1)

nexttile;
LeftLine = animatedline;
xlabel('ID');
ylabel('Left (real units)');
title('Left Wheel (converted)');
grid on;

nexttile;
RightLine = animatedline;
xlabel('ID');
ylabel('Right (real units)');
title('Right Wheel (converted)');
grid on;

nexttile;
DirLine = animatedline;
xlabel('ID');
ylabel('Direction (R - L)');
title('Direction Indicator');
grid on;

% =========== Read and process data - We will start from ID2
while (true)

    data=readline(s);

    % Get (via a function) the ID, Left and Right counts
    [Id, LeftCount, RightCount] = parseData(data);

    % Check for a termination condition (for example, a specific ID)
    if Id == terminationID 
        disp('Termination ID received. Exiting loop.');
        break; % Exit the inner while loop
    end

    % Process the counts (for example, display them)
    fprintf('ID: %d, Left Count: %d, Right Count: %d\n', Id, LeftCount, RightCount);

    % Store the counts in an array for later plotting
    IdData = [IdData ; Id];
    LeftData = [LeftData; LeftCount];
    RightData = [RightData; RightCount];

    % Calculate distances based on counts and scale
    LeftDistance = LeftCount * distancePerClick * distanceScale;
    RightDistance = RightCount * distancePerClick * distanceScale;

    % Update total distance data
    LeftDistTotalData = [LeftDistTotalData; LeftDistance];
    RightDistTotalData = [RightDistTotalData; RightDistance];

    % Update direction data
    DirectionData = [DirectionData; RightDistance - LeftDistance];
    %disp(DirectionData);

    % Update animated lines for plotting
    addpoints(LeftLine, Id, LeftDistance);
    addpoints(RightLine, Id, RightDistance);
    addpoints(DirLine, Id, DirectionData(end));
    drawnow;

    % Check for direction changes based on distance thresholds
    if abs(LeftDistance - RightDistance) > directionThreshold
        if LeftDistance < RightDistance
            TurnTextData(end + 1) = "RIGHT";
        else
            TurnTextData(end + 1) = "LEFT";
        end
    else
        TurnTextData(end + 1) = "STRAIGHT";
    end

    % Display the turn direction for the current ID
    disp(['ID: ', num2str(Id), ' - Turn Direction: ', TurnTextData(end)]);

end

function [Id, LeftCount, RightCount] = parseData(data)

% Parse by comma first
strParseStage1 = split(data,",");

% Now we need to extract the individual components from the parsed data
Id = str2double(extractAfter(strParseStage1{1}, 'ID:'));
LeftCount = str2double(extractAfter(strParseStage1{2}, 'LEnc:'));
RightCount = str2double(extractAfter(strParseStage1{3}, 'REnc:'));

end

% Close the serial port after exiting the loop
clear s;
disp('Serial port closed. Exiting program.');




