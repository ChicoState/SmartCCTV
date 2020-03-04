##########################################################################################################
# The purpose of this is to compare two 24-hour periods which is abstracted from the logfiles designated # 
# as "in.txt" and "inSec.txt". The format will allow for 'x' amount of comparisons given 'x' is less     #
# than a number that would simply convolute the graph further, making it unreadable.                     # 
##########################################################################################################

library(ggplot2)
library(tidyr)
library(dplyr)

# Designate files for I/O
src <- file("in.txt", "rb")
srcSec <- file("inSec.txt", "rb")

# Get line by line input from 'src' file
count <- 1
times <- c()
while (length(cLine <- readLines(src, warn=FALSE, n=1)) > 0) {
	# Split string into a vector
	cList <- strsplit(cLine, " ")

	# Check for event type, can be used as error checking
	if (cList[[1]][9] == "ACTIVITY") {
		cTime <- strsplit(cList[[1]][5], ":") 
		cNum <- as.numeric(cTime[[1]][1]) + (as.numeric(cTime[[1]][2]) * 1/60) + (as.numeric(cTime[[1]][3]) * 1/3600)
		times[count] <- cNum
		count <- count + 1
	}
}
close(src)

# Get secondary input
timesSec <- c()
count <- 1
while (length(cLine <- readLines(srcSec, warn=FALSE, n=1)) > 0) {
	# Split string into a vector
	cList <- strsplit(cLine, " ")

	# Check for event type ('ACTIVITY' VS 'ERROR')
	if (cList[[1]][9] == "ACTIVITY") {
		# Insert converted data (cList[[1]][5]) into our vector and increment the counter for vector indexing
		cTime <- strsplit(cList[[1]][5], ":") 
		timesSec[count] <- as.numeric(cTime[[1]][1]) + (as.numeric(cTime[[1]][2]) * 1/60) + (as.numeric(cTime[[1]][3]) * 1/3600)
		count <- count + 1
	}
}
close(srcSec)
  
# PDF/Plot generation
pdf("activity-plot.pdf")

# Set the base DF and the DF for mean values
df <- data.frame("{DATE 1}"=sample(times, length(times), replace = FALSE), "{DATE 2}"=sample(timesSec, length(timesSec), replace = FALSE))
df_lab <- df %>% pivot_longer(everything(), names_to = "var",values_to = "val") %>% group_by(var) %>% summarise(Mean = mean(val), Density = max(density(val)$y))

# Create the graph
df %>% pivot_longer(everything(), names_to = "var", values_to = "val") %>%
	# Base graph info
	ggplot(aes(x = val, fill = var, colour = var)) +
	geom_density(alpha = 0.8) +
	theme(legend.position="top") +

	# Data Set 2 MISC.
	scale_color_manual(values = c("#4271AE", "#FF7F00")) +
	scale_fill_manual(values = c("#1A3552", "#D3D3D3")) +

	# Mean vertical lines & assoc. text
	geom_vline(inherit.aes = FALSE, data = df_lab, aes(xintercept = Mean, color = var), linetype = "dashed", size = 1, show.legend = FALSE) +
	geom_text(inherit.aes = FALSE, data = df_lab, aes(x = Mean-0.5, y = Density/2, label = var, color = var), angle = 90, show.legend = TRUE) +

	# X-Y axis & Labels
	scale_x_continuous(name = "Time of Day", breaks=seq(c(0:23))) + 
	scale_y_continuous(name = "Activity") +
	labs(title="Activity in the past 48 hours", subtitle="From {DATE 1} to {DATE 2}", caption="{LOCATION}")
dev.off()