# Computes the edit distance between 2 strings. The allowed operations are
# insertions, deletions, and substitutions.

s = "SNOWY"
t = "SUNNY"

def diff(i,j):
	if(s[i] == t[j]):
		return 0
	else:
		return 1

def editDistance(i,j):
	if(j == 0):
		return 1
	if(i == 0):
		return 1
	return(min(1 + editDistance(i,j-1),1 + editDistance(i-1,j),diff(i,j) + editDistance(i-1,j-1)))

i = s.__len__()-1
j = t.__len__()-1

print(editDistance(i,j))