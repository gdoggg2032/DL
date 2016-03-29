path1 = "./48-idx.map"
path2 = "./48_39.map"


def makeMapping(path):
	#mapping maps to char like 'sil'
	#while mapping character maps to a~V
	mapping = {}
	mappingCharacter = {}
	
	with open(path1, "r") as f:
		for i, line in enumerate(f):
			tmp = line.split()
			mapping[i] = tmp[0]
	with open(path2, "r") as f:
		for i, line in enumerate(f):
			tmp = line.split()
			mappingCharacter[tmp[0]] = tmp[1]
			
	return mapping, mappingCharacter


if __name__ == "__main__":
	mapping, mappingCharacter = makeMapping(path)
	for i in range(len(mapping)):
		print mapping[i], mappingCharacter[i]