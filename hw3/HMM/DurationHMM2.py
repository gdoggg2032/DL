import math
import numpy as np
import os 

class HMM(object):
	def __init__(self, input, label, testData):
		self.input = input
		self.label = label
		
	def calTransitionProb(self):

		
		'''
		transition probability
		'ones' for smoothing; there might be no any transition from one state to another.
		'''
		self.transProb = np.ones((48, 48))

		

		'''
		state probability, 
		namely the probability of each label
		'''
		self.prob = np.zeros(48)

		total = len(self.label)
		print "Total labels: ", total

		
		for i in range(total - 1):
			self.transProb[self.label[i]][self.label[i + 1]] += 1
			self.prob[self.label[i]] += 1

		# Last one
		self.prob[self.label[total - 1]] += 1




		for i in range(48):
			self.prob[i] = self.prob[i] / float(total)
			transitionTotal = 0.0
			for j in range(48):
				transitionTotal += self.transProb[i][j]

			'''
			Normalizing
			'''
			for j in range(48):
				self.transProb[i][j] /= transitionTotal
		
	def viterbi(self, testData):
		'''
		Accumulated probability for each state
		Not implemented in log domain though
		Because the length is only 100, not going to be underflow.
		'''
		logProb = []
		labels = []

		'''
		Omitting the start probability
		'''
		for i in range(48):
			logProb.append(math.log(float(testData[0][i])) * 3 + math.log(self.transProb[i][i]) * 2)
			labels.append([i, i, i])

		totalData = len(testData)
		idx = 3
		while idx < totalData:
			newLabels = []
			newLogProb = []
			for j in range(48):
				newLabels.append([])
				newLogProb.append(0)
				bestProb = float('-inf')
				best_i = 0
				for i in range(48):
					tmp = logProb[i] + math.log(self.transProb[i][j])
					
					if tmp > bestProb:
						bestProb = tmp
						best_i = i
				
				### Now without dividing p(j) ###
				newLogProb[j] = logProb[best_i] + math.log(self.transProb[best_i][j]) + math.log(self.transProb[j][j]) * 2 \
								 + sum_state_given_ob(testData, idx, j, totalData) 


				newLabels[j] = labels[best_i] + [j] * min(3, totalData - idx)
				
				#print totalData
			for j in range(48):
				labels[j] = newLabels[j]
				logProb[j] = newLogProb[j]
			idx += 3
		
		resultProb = max(logProb)
		argmaxIdx = np.argmax(logProb)
		print argmaxIdx
		return labels[argmaxIdx], resultProb

def sum_state_given_ob(testData, idx, state, totalData):
	total = 0
	for i in range(min(3, totalData - idx)):
		emission = float(testData[idx + i][state])
		Max = max([float(x) for x in testData[idx + i]])
		if Max == emission and Max < 0.7:
			emission *= 0.8
		elif Max != emission and Max < 0.7:
			emission += Max * 0.2 / 47.0
		total += math.log(emission)
	return total


def loadTrainData(filePath):
	label = []
	input = []
	# with open(filePath, "r") as f:
	# 	for line in f:
	# 		tmp = line.strip().split()
	# 		label.append(tmp.pop(0))
	# 		input.append([i.split(":")[1] for i in tmp])
	maps = mapping("48-idx.map")
	with open(filePath, "r") as f:
		for line in f:
			label.append(maps[line.split()[1]])
	print len(label)

	return label, input


'''
From our mapping to JN mapping
'''
def mapping(filePath):
	mapping = {}
	with open(filePath, "r") as f:
		for line in f:
			tmp = line.strip().split()
			mapping[tmp[0]] = int(tmp[1])
	return mapping

def loadTestData(filePath, SeqFile):

	seqlength = []
	with open(SeqFile, "r") as f:
		for i in f:
			seqlength.append(int(i))

	cnt = 0
	idx = 0 
	testData = [[]]
	with open(filePath, "r") as f:
		for i, line in enumerate(f):
			
			if cnt == seqlength[idx]:
				testData.append([])
				cnt = 0
				idx += 1
			cnt += 1
			testData[idx].append([feature for feature in line.strip().split()])
	print len(testData[0])
	return testData

def writeToTest(path, predicted):
	f = open(path, "w")
	for i in predicted:
		f.write(str(i) + "\n")



if __name__ == "__main__":
	label, trainData = loadTrainData("NewTrainLabel")
	testData = loadTestData("newTestFile.txt", "SeqLength.txt")
	myHMM = HMM(trainData, label, testData)
	myHMM.calTransitionProb()
	predicted = []
	results = []
	for i in range(len(testData)):
		print i
		p, r = myHMM.viterbi(testData[i])
		predicted += p
		print r



	print "Writing test results to file"
	writeToTest("testResult.txt", predicted)
	print "Done writing"












