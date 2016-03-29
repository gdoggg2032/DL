import sys

testingFile = open("MLDS/fbank/test.ark", "r")
resultFile = open("Mtest.ans", "r")

outputFile = open("outputN3_256_mom.csv", "w")

outputFile.write("ID,Prediction\n")

while True:
	tmpLine = testingFile.readline()
	if tmpLine == "":
		break
	ID = tmpLine.split()[0]
	outputFile.write(ID + ",")
	tmpLine = resultFile.readline().strip()
	outputFile.write(tmpLine + "\n")