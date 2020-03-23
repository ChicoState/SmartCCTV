 # File Name:  document.R
 # Created By:  Connor Adams
 # Created On:  2/03/20
 #
 # Description:
 # The purpose of this is to compare 'N' amount of 24-hour periods, which is extracted from the
 # logfiles designated in command line call as arguements. The format will allow for 'N'
 # amount of data sets. However, it should be noted that 'N' shall not exceed ~5, given that any
 # more than that would simply convolute the graph further, making it unreadable.

# Import libraries
library(ggplot2)
library(tidyr)
library(dplyr)
library(stringr)

# Define data collection function
getInput <- function(times, input) {
    # Designate file for I/O
    src <- file(input, "rb")
    
    # Get line by line input from 'input' file
    count <- 1
    data <- c()
    while (length(cLine <- readLines(src, warn=FALSE, n=1)) > 0) {
        # Split string into a vector
        cList <- strsplit(cLine, " ")
        
        # Check for event type (See example line(s) provided for sample)
        if (str_detect(cLine, "camera\\[[0-9]{1,}\\]:\ [:print:]{0,}Motion")) {
            # Insert converted data into our vector and increment the counter
            cTime <- strsplit(cList[[1]][5], ":") 
            data[count] <- (
                (as.numeric(cTime[[1]][1])) + 
                (as.numeric(cTime[[1]][2]) * 1/60) + 
                (as.numeric(cTime[[1]][3]) * 1/3600)
            )
            count <- count + 1
        }
    }
    close(src)
    return(data)
}

getColour <- function(i) {
    set.seed(i)
    return(paste0(sample(c(0:9, LETTERS[1:6]), 6, T), collapse = ''))
}

# Set the arguement to trailing only and perform 
# a security check on arguements length
args = commandArgs(trailingOnly = TRUE)
if (length(args) < 1) {
    stop("ERROR: Invalid number of arguements, please try specifying input files")
}

# Process the arguements into function calls of getInput()
times <- list()
for (i in 1:length(args)) {
    times[[i]] <- getInput(times, args[i])
}

# Start PDF/Plot generation
pdf("activity-plot.pdf")

# Set the base DF (only 2 by default) and the associated labels 
df <- data.frame(
    "{DATE 1}"=sample(times[[1]], length(times[[1]]), replace = FALSE), 
    "{DATE 2}"=sample(times[[2]], length(times[[2]]), replace = FALSE)
)
df_lab <- summarise(
    group_by(pivot_longer(df, everything(), names_to = "var",values_to = "val"), var), 
    Mean = mean(val), 
    Density = max(density(val)$y)
)

# Create the graph by passing in the DF and adding to the information sequentially
ggplot(pivot_longer(df, everything(), names_to = "var", values_to = "val"), aes(x = val, fill = var, colour = var)) +
    # Base graph info
    geom_density(alpha = 0.8) +
    theme(legend.position="top") +
    
    # Plot colours
    scale_color_manual(values = c(
            sprintf("#%s", getColour(sample(1:10000, 1))), 
            sprintf("#%s", getColour(sample(1:10000, 1))))
    ) +
    scale_fill_manual(values = c(
            sprintf("#%s", getColour(sample(1:10000, 1))), 
            sprintf("#%s", getColour(sample(1:10000, 1))))
    ) +
    
    # Mean grouping vertical lines
    geom_vline(data = df_lab, aes(xintercept = Mean, color = var), linetype = "dashed", size = 1, show.legend = FALSE) +
    geom_text(inherit.aes = FALSE, data = df_lab, aes(x = Mean-0.5, y = Density/2, label = var, color = var), angle = 90, show.legend = TRUE) +
    
    # X-Y axis & Labels
    scale_x_continuous(name = "Time of Day", breaks=seq(c(0:23))) + 
    scale_y_continuous(name = "Activity") +
    labs(
        title=sprintf("Activity in the last %i hours", (ncol(df)) * 24), 
        subtitle=sprintf("From %s to %s", argv[1], argv[2]), 
        caption="{LOCATION OF CAMERA}"
    )

# End PDF/Plot generation
dev.off()
