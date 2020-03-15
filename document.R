#################################################################################################
# The purpose of this is to compare 'N' amount of 24-hour periods, which is extracted from the  #
# logfiles designated in command line call as arguements. The format will allow for 'N'         #
# amount of data sets. However, it should be noted that 'N' shall not exceed ~5, given that any #
# more than that would simply convolute the graph further, making it unreadable.                #
#################################################################################################

# Import libraries
library(ggplot2)
library(tidyr)
library(dplyr)

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
        
        # Check for event type
        if (cList[[1]][9] == "ACTIVITY") {
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

getDate <- function(fname) {
    # getDate() will be finished upon completion of format for log file names
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

# Set the base DF (only 2 by default) and the labels for mean values of DF
df <- data.frame(
    "{DATE 1}"=sample(times[[1]], length(times[[1]]), replace = FALSE), 
    "{DATE 2}"=sample(times[[2]], length(times[[2]]), replace = FALSE)
)
df_lab <- df %>% pivot_longer(everything(), names_to = "var",values_to = "val") %>% group_by(var) %>% summarise(Mean = mean(val), Density = max(density(val)$y))

# Create the graph by passing in the DF and adding to the information sequentially
df %>% pivot_longer(everything(), names_to = "var", values_to = "val") %>%
ggplot(aes(x = val, fill = var, colour = var)) +
    # Base graph info
    geom_density(alpha = 0.8) +
    theme(legend.position="top") +
    
    # Data set colourings (WILL THROW ERROR AT INCORRECT # OF ARGS)
    scale_color_manual(values = c("#4271AE", "#FF7F00")) +
    scale_fill_manual(values = c("#1A3552", "#D3D3D3")) +
    
    # Mean grouping vertical lines (ALL DATA SETS)
    geom_vline(data = df_lab, aes(xintercept = Mean, color = var), linetype = "dashed", size = 1, show.legend = FALSE) +
    geom_text(inherit.aes = FALSE, data = df_lab, aes(x = Mean-0.5, y = Density/2, label = var, color = var), angle = 90, show.legend = TRUE) +
    
    # X-Y axis & Labels
    scale_x_continuous(name = "Time of Day", breaks=seq(c(0:23))) + 
    scale_y_continuous(name = "Activity") +
    labs(title="Activity in the past {(ncol(df)) * 24} hours", subtitle="From {df[1]} to {df[ncol(df)]}", caption="{LOCATION}")

# End PDF/Plot generation
dev.off()
