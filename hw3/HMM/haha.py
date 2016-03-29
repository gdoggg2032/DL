import mapping
mapping, mappingCharacter = mapping.makeMapping('./48_idx_chr.map_b')
f1 = open("./testResult.txt","r")
f2 = open("./test.ark","r")
p = open("try.test.out","w")



L = []
for line in f2:
	L.append(line.strip().split()[0])

for i ,line in enumerate(f1):
	a = int(line.strip())
	if i == 0:
		print a
	output = L[i]+','+mappingCharacter[mapping[a]]+'\n'
	p.write(output)




