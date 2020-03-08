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
			data[count] <-  (as.numeric(cTime[[1]][1]) +          # Hours
							(as.numeric(cTime[[1]][2]) * 1/60) +  # Minutes
							(as.numeric(cTime[[1]][3]) * 1/3600)) # Seconds
			count <- count + 1
		}
	}
	close(src)
	return(data)
}	

# Get data from command line arguments
args = commandArgs(trailingOnly = TRUE)
times <- list()
for (i in 1:length(args)) {
	times[[i]] <- getInput(times, args[i])
}

# PDF/Plot generation
pdf("activity-plot.pdf")

# Set the base DF and the DF for mean values (Only allowing 2 24-hr comparisons by default)
df <- data.frame("{DATE 1}"=sample(times[[1]], length(times[[1]]), replace = FALSE), "{DATE 2}"=sample(times[[2]], length(times[[2]]), replace = FALSE))
df_lab <- df %>% pivot_longer(everything(), names_to = "var",values_to = "val") %>% group_by(var) %>% summarise(Mean = mean(val), Density = max(density(val)$y))

# Create the graph by passing in the DF and adding to the information sequentially
df %>% pivot_longer(everything(), names_to = "var", values_to = "val") %>%
	# Base graph info
	ggplot(aes(x = val, fill = var, colour = var)) +
	geom_density(alpha = 0.8) +
	theme(legend.position="top") +

	# Data set 2 colourings
	scale_color_manual(values = c("#4271AE", "#FF7F00")) +
	scale_fill_manual(values = c("#1A3552", "#D3D3D3")) +

	# Mean grouping vertical lines
	geom_vline(data = df_lab, aes(xintercept = Mean, color = var), linetype = "dashed", size = 1, show.legend = FALSE) +
	geom_text(inherit.aes = FALSE, data = df_lab, aes(x = Mean-0.5, y = Density/2, label = var, color = var), angle = 90, show.legend = TRUE) +

	# X-Y axis & Labels
	scale_x_continuous(name = "Time of Day", breaks=seq(c(0:23))) + 
	scale_y_continuous(name = "Activity") +
	labs(title="Activity in the past 48 hours", subtitle="From {DATE 1} to {DATE 2}", caption="{LOCATION}")
dev.off()
