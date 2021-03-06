 # File Name:  document.R
 # Created By:  Connor Adams
 # Created On:  2/03/20
 #
 # Description:
 # The purpose of this is to compare 'N' amount of 24-hour periods, which is extracted from the
 # logfiles designated in command line call as arguements. The format will allow for 'N'
 # amount of data sets. However, it should be noted that 'N' shall not exceed 7, given that any
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
            # (Seconds) + (Minutes * 1/60) + (Hours * 1/3600)
            cTime <- strsplit(cList[[1]][3], ":") 
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

# Set the arguement to trailing only and perform 
# a security check on arguements length
args = commandArgs(trailingOnly = TRUE)
if (length(args) < 1 || length(args) > 7) {
    stop("ERROR: Invalid number of arguements, please verify your input")
}

# Process the arguements into function calls of getInput()
times <- list()
for (i in 1:length(args)) {
    times[[i]] <- getInput(times, args[i])
}

# Start PDF/Plot generation
pdf("Overview.pdf")

# Set main dataframe
df <- data.frame("Day.1"=sample(times[[1]], length(times[[1]]), replace = FALSE))
if (length(args) > 1) {
    for (i in 2:length(args)) {
        df$i <- sample(times[[i]], length(times[[1]]), replace = FALSE)
        names(df)[names(df) == "i"] <- sprintf("Day.%i", i)
    }
}

# Set the dataframe labels
df_lab <- summarise(
    group_by(pivot_longer(df, everything(), names_to = "Group",values_to = "val"), Group),
    Mean = mean(val),
    Density = max(density(val)$y)
)

# Plot density of activity
ggplot(pivot_longer(df, everything(), names_to = "Group", values_to = "val"), aes(x = val, fill = Group, colour = Group)) +
    # Base graph info and theme
    geom_density(alpha = 0.8) +
    theme_minimal() +
    scale_fill_brewer(palette="Pastel1") +
    scale_color_brewer(palette="Pastel1") +

    # Mean grouping vertical lines
    geom_vline(data = df_lab, aes(xintercept = Mean, color = Group), linetype = "dashed", size = 1, show.legend = FALSE) +
    geom_text(inherit.aes = FALSE, 
              data = df_lab, 
              aes(x = Mean-0.5, y = Density/2, label = sprintf("Peak Hour -- %i:00", as.integer(Mean)), alpha=0.90), 
              angle = 90, 
              show.legend = FALSE,
              size = 3
    ) +
    
    # X-Y axis & Labels
    scale_x_continuous(name = "Time of Day", breaks=seq(c(0:23))) + 
    scale_y_continuous(name = "") +
    labs(
        title=sprintf("Activity in the last %i hours", (ncol(df)) * 24), 
        subtitle=sprintf("From '%s' to '%s'", args[1],  args[length(args)]), 
        caption=sprintf("Overall Peak hours -- %i:00", as.integer(mean(df_lab$Mean)))
    )

# Plot peak hours by day
df <- data.frame("Day"=df_lab$Group,"Time"=df_lab$Mean)
ggplot(data=df, aes(x=Day, y=Time, fill=Time)) +
    geom_bar(stat="identity") +

    # Labels and theme information
    geom_text(aes(label=as.integer(Time)), vjust=-0.3, size=3, alpha=0.90) +
    theme_minimal() + 
    labs(
        title=sprintf("Peak hours by day", (ncol(df)) * 24), 
        subtitle=sprintf("From '%s' to '%s'", args[1],  args[length(args)]), 
        caption=sprintf("Overall Peak hours -- %i:00", as.integer(mean(df_lab$Mean)))
    )

# End PDF/Plot generation
dev.off()
