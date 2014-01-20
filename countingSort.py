#input A[0,...,n]
	#A[j] E {1,2,..,k}
#output B[0,...,n]
#uses C[0,...,k]
def countingSort(A):
	C = []
	i = 0
	while i < max(A):#init C
		C.append(0)
		i+=1

	B = []
	i = 0
	while i < A.__len__():#init B
		B.append(0)
		i+=1

	for i in A:#C now holds th number of occurences of each number
		C[i-1] += 1

	i = 1
	while i < max(A):#C[i] not holds no of elements <= i
		C[i] = C[i] + C[i-1]
		i+=1

	i = A.__len__() - 1
	while i >= 0:#Read off C in the correct order into B
		B[C[A[i]-1]-1]=A[i]
		C[A[i]-1]-=1
		i -= 1

	return B

inputA = [7,5,6,5,2,1,7,4,3]
print(countingSort(inputA))
