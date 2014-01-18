#Recursive function to compute the optimal
#subsequence of a list of numbers, where no
#elements in the subsequence can be next to
#eachother in the original list

a = [2,3,5,1,6]
n = a.__len__() - 1

def OPT(j):
	if(a[j] == a[n] or a[j] == a[n-1]):
		return a[j]

	return max(a[j] + OPT(j+2),OPT(j+1))

print(OPT(0))