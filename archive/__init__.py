import cv2
import numpy as np

ERR_OUT_OF_RANGE=-1
ERR_OPEN_IMG=-2

def f(pth:str,mx:int=2,resize:tuple=None)->list:
	if mx<1 or mx>8:
		return ERR_OPEN_RANGE

	_head_and=(1<<mx)-1
	_tail_and=(1<<(8-mx))-1
	_mid=1<<(7-mx)

	print('Loading...')

	try:
		img=cv2.imread(pth,cv2.IMREAD_UNCHANGED)
		if resize:
			img=cv2.resize(img,tuple(resize),cv2.INTER_AREA)
		for i in img:
			break
	except:
		return ERR_OPEN_IMG

	a=list()
	for i in range(1<<(mx*3)):
		a.append([0,0,0,0,])

	print('Analyzing...')

	for i in img:
		for j in i:
			k=(j[0]>>(8-mx))|(j[1]>>(8-mx)<<mx)|(j[2]>>(8-mx)<<(mx<<1))
			for l in range(3):
				a[k][l]+=(j[l]&_tail_and)-_mid
			a[k][3]+=1

	print('Calculating...')

	_c2head=lambda c,k:((c>>(mx*k))&_head_and)<<(8-mx)
	_div=lambda a,b:(a+(b>>1))//b

	ans=list()
	for c,i in enumerate(a):
		if i[3]:
			for k in range(3):
				i[k]=_div(i[k],i[3])+_c2head(c,k)+_mid
			ans.append((i[3],i[2]<<16|i[1]<<8|i[0]))

	print('Sorting...')
	ans.sort(key=lambda x:-x[0])

	return ans

if __name__=="__main__":
	
	for i in f('87011701_p0.jpg.original.bmp'):
		print('#'+hex(i[1])[2:],i[0])
	# print(f('87011701_p0.jpg',resize=(32,32)))