#!/usr/bin/env/Rscript
library(ggplot2)
library(grid)
library(scales)
library(RColorBrewer)

args = commandArgs(trailingOnly=TRUE)
inputfile = args[1]
num_thread = args[2]
substrings = strsplit(inputfile, "\\/")
listsize = length(substrings[[1]])
filename=substrings[[1]][listsize]
substrings = strsplit(filename, "\\.")
targetName = substrings[[1]][1]

path = paste(getwd(), "/", sep="")

# read data
data = read.table(inputfile, header=T, sep=",")

data$ExecutionTime <- data$ExecutionTime/1000000
data$SD <- data$SD/1000000
data$Mean <- data$Mean/1000000
data$Throughput <- as.double(data$Throughput)
data$TH_SD <- as.double(data$TH_SD)

paletteSet=c("#000075","#F032E6","#808000", "#9A6324","#F58231","#3CB44B","#4363D8","#E6194B", "#469990", "#CC0000", "#33FF00", "#1B9E77", "#D95F02", "#7570B3")
commonPlotMargin=unit(c(1,1,0,1), "mm")


numLegendRows = 1

##########################
# Build ExecutionTime plot
##########################
PointSize = 1.5
LineWidth = 0.5
ExecutionTimePlot = ggplot(data, aes(x=Threads, y=ExecutionTime, fill=Version, colour=Version, shape=Version)) +
    geom_point(size=PointSize) +
    geom_line(size=LineWidth) +
    geom_errorbar(aes(ymin=ExecutionTime-SD, ymax=ExecutionTime+SD), width=.1, position=position_dodge(0.9)) +
    scale_shape_manual(values=seq(0,16), guide=guide_legend(nrow=numLegendRows)) +
    scale_colour_manual(values=paletteSet) +
    xlab("Number of Threads") +
    ylab("Execution Time (milliseconds)") +
    scale_x_continuous(breaks=seq(1,num_thread,1)) +
    scale_y_continuous() +
    theme_bw() +
    theme(legend.position="bottom") +
    #theme(legend.title=element_blank(), legend.text=element_text(size=15)) +
    theme(legend.title=element_blank(), legend.text=element_text(size=15)) +
    theme(axis.title.x=element_text(size=15,vjust=-0.4)) +
    theme(axis.title.y=element_text(size=15,vjust=+0.4)) +
    #theme(axis.text.x=element_text(size=10,hjust=+1.0,angle=60)) +
    theme(axis.text.x=element_text(size=15,hjust=+1.0,angle=60)) +
    theme(axis.text.y=element_text(size=16,vjust=+0.9)) +
    theme(legend.spacing.x = unit(0, unit="mm")) +
    theme(plot.margin = commonPlotMargin)######

#######################
# Build Throughput plot
#######################
yLabel = "Throughput (tuples/second)"
base_breaks <- function(n = 10) {
  function (x) {
    axisTicks(log10(range(x, na.rm=TRUE)), log=TRUE, n=n)
  }
}

PointSize = 1.6
LineWidth = 0.4
ThroughputPlot = ggplot(data, aes(x=Threads, y=Throughput, fill=Version, colour=Version, shape=Version)) +
    geom_point(size=PointSize) +
    geom_line(size=LineWidth) +
    geom_errorbar(aes(ymin=Throughput-TH_SD, ymax=Throughput+TH_SD), width=.2, position=position_dodge(0.9)) +
    scale_shape_manual(values=seq(0,16), guide=guide_legend(nrow=numLegendRows)) +
    scale_colour_manual(values=paletteSet) +
    xlab("Number of Threads") +
    ylab("Throughput (tuples/second)") +
    scale_x_continuous(breaks=seq(1,num_thread,1), expand=c(0.000, 0)) +
    #scale_y_continuous() +
#####scale_y_continuous(trans="log10", breaks=base_breaks(), labels=prettyNum) +
    scale_y_continuous(trans="log10", breaks=trans_breaks("log10", function(x) 10^x), labels=trans_format("log10", math_format(10^.x))) +
    annotate(geom="text", x=2, y=1*(10^6.1), label = "(log10-scale)", vjust=-1.35, hjust=0.080,size=6.2,angle=90, fontface = "italic") +
    theme_bw() +
    theme(legend.position="bottom") +
    theme(legend.title=element_blank(), legend.text=element_text(size=15)) +
    theme(axis.title.x=element_text(size=25,vjust=-0.4)) +
    theme(axis.title.y=element_text(size=25,vjust=+0.4)) +
    theme(axis.text.x=element_text(size=22,hjust=+1.0,angle=60)) +
    theme(axis.text.y=element_text(size=22,vjust=+0.9)) +
    theme(legend.spacing = unit(0, unit="mm")) +
    theme(plot.margin = commonPlotMargin)######

# Export to gra, p)#########################
exportTo <- function(path, targetName, plot) {
  output_pdf = paste(targetName, ".pdf", sep="")
  output_svg = paste(targetName, ".svg", sep="")
  output_png = paste(targetName, ".png", sep="")

  ggsave(paste(path, output_pdf, sep=""), plot, width=10, height=7, scale=1.00, limitsize=FALSE)
  ggsave(paste(path, output_png, sep=""), plot, width=10, height=7, scale=1.00, limitsize=TRUE)
}

Exec_targetName=paste(targetName,"_ExecutionTime",sep="")
exportTo(path, Exec_targetName, ExecutionTimePlot)
Thr_targetName=paste(targetName,"_Throughput",sep="")
exportTo(path, Thr_targetName, ThroughputPlot)
