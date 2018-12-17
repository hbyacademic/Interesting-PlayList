clc;    % Clear the command window.
close all;  % Close all figures (except those of imtool.)
imtool close all;  % Close all imtool figures.
clear;  % Erase all existing variables.
workspace;  % Make sure the workspace panel is showing.

fontSize = 15;
flag=1;
movieName='view2.asf';
 
try
	InputVideo = VideoReader(movieName);
	% find the number of the frame in movie
	FrameAmount = InputVideo.NumberOfFrames;
    
    % Prepare to show the image on screen
	figure;
	% full screen
	set(gcf, 'units','normalized','outerposition',[0 0 1 1]);
    
    % the first frame
    thisFrame = read(InputVideo, 1);
    % gray = r*0.299 + g*0.587 + b*0.114
    grayImage = rgb2gray(thisFrame);
	prev = mean(grayImage(:));

    %vinitialization
	meanGrayLevels = zeros(FrameAmount, 1);
    
	for frame = 1 : FrameAmount
        % previous frame of the first frame is itself
        if frame==1; prev_idx=1;   
        else; prev_idx=frame-1; end
        
        % read the i-th frame
        thisFrame = read(InputVideo, frame);
        grayImage = rgb2gray(thisFrame);
		meanGrayLevels(frame) = mean(grayImage(:));
        % read the (i-1)-th frame
        prevFrame = read(InputVideo, prev_idx);
        
        %{ 
            subplot(2,4,x)
            -----------------
            | 1 | 2 | 3 | 4 |
            -----------------
            | 5 | 6 | 7 | 8 |
            -----------------
        %}
        
        % find the first key frame that has any difference with the previous frame
        if (abs(prev-meanGrayLevels(frame))>0.1 && flag)
            hImage = subplot(2, 4, 4);
            image(thisFrame);
            caption=sprintf('Screenshot: %dth frame',frame);
            title(caption,'FontSize', fontSize);
            flag=0;
            
            hImage = subplot(2, 4, 8);
            imshowpair(thisFrame,prevFrame,'diff');
            title('Where is the difference(enemy)? ', 'FontSize', fontSize);
        end 
        prev=meanGrayLevels(frame); 
        
		% extract all the frames from movie (that is,Image)
		hImage = subplot(2, 4, [1 2 5 6]);
		image(thisFrame);
		caption = sprintf('Frame: %dth / %dth', frame, FrameAmount);
		title(caption, 'FontSize', fontSize);
        % force it to refresh the window
		drawnow; 

		% plot mean gray level of all the frames
		hPlot = subplot(2, 4, [3 7]);
		hold off;
        plot(meanGrayLevels, 'k-'); 
		hold on;
        grid on;
		title('Mean Gray Level', 'FontSize', fontSize);
        
    end
    
catch ME
	% Some error happened if you get here.
	strErrorMessage = sprintf('Error extracting movie frames from:\n\n%s\n\nError: %s\n\n)', movieName, ME.message);
	uiwait(msgbox(strErrorMessage));
end


