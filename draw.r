args<-commandArgs(TRUE)

data <- read.csv(args[1])
x = seq(1, nrow(data), by=1)
pdf(paste(args[2], 'pdf', sep="."), encoding="ISOLatin2")
plot(c(1, max(data[,c("Gen_No")]), 1), c(0.4,max(data[,c("Training", "Test")]), 1), main=paste("Prikaz rezultata za", args[2], sep=" "), xlab="Broj generacije", ylab=paste(args[3], "u %", sep=" "), type="n")
lines(x, data$Test, col="red");
lines(x, data$Training, col="blue")
legend("bottomright", legend=c("Skup za učenje", "Skup za testiranje"), horiz=FALSE, lty=c(1,1), col=c("blue", "red"))
dev.off()
