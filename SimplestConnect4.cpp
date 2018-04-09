extern "C" __declspec(dllexport) Point* getPoint(const int M, const int N, const int* top, const int* _board, 
	const int lastX, const int lastY, const int noX, const int noY){
	/*
		不要更改这段代码
	*/
	int x = -1, y = -1;//最终将你的落子点存到x,y中
	int** board = new int*[M];
	for(int i = 0; i < M; i++){
		board[i] = new int[N];
		for(int j = 0; j < N; j++){
			board[i][j] = _board[i * N + j];
		}
	}
	
	/*
		根据你自己的策略来返回落子点,也就是根据你的策略完成对x,y的赋值
		该部分对参数使用没有限制，为了方便实现，你可以定义自己新的类、.h文件、.cpp文件
	*/
	//Add your own code below

     //a naive example
	/*
	for (int i = N-1; i >= 0; i--) {
		if (top[i] > 0) {
			x = top[i] - 1;
			y = i;
			break;
		}
	}*/

	int* plays=new int[N]();
	int* wins=new int[N]();
	srand(time(NULL));
	int iter=0;
	int** simulate = new int*[M];
	int* top_new=new int[N];
	for(int i=0; i<N; i++){
		top_new[i]=top[i];
	}
	
	while(iter<10){
		iter++;
		int ct=rand()%N;
		/*
		swprintf(buff,100,L"in main %d",ct);
		MessageBox(NULL, buff, L"Msg title", MB_OK | MB_ICONQUESTION);*/
		for(int i = 0; i < M; i++){
			simulate[i] = new int[N];
			for(int j = 0; j < N; j++){
				simulate[i][j] = _board[i * N + j];
			}
		}
		simulateOnce(simulate, M, N, plays, wins, top_new, noX, noY, ct);

		for(int i = 0; i < M; i++){
			delete[] simulate[i];
		}
	}
	
	float max=0;
	float curr=0;
	int choice=0;
	
	for(int i=0; i<=N;i++){
		if(plays[i]==0)
			curr=0;
		else{
			curr=(float)wins[i]/plays[i];
		}
		if(curr>max){
			choice=i;
			max=curr;
		}
	}
	y=choice;
	x=top[choice]-1;
	if(choice>=N){
	
		for (int i = N-1; i >= 0; i--) {
			if (top[i] > 0) {
				x = top[i] - 1;
				y = i;
				break;
			}
		}
	}
	delete[] plays;
	delete[] wins;
	delete[] top_new;
	delete[] simulate;
	/*
		不要更改这段代码
	*/
	clearArray(M, N, board);

	return new Point(x, y);
}

void simulateOnce(int** simulate, int M, int N, int* play, int* wins, int* top, int noX, int noY, int ct){
	/*
		swprintf(buff,100,L"in simulation %d",ct);
		MessageBox(NULL, buff, L"Msg title", MB_OK | MB_ICONQUESTION);
	*/
	if(top[ct]==0)
		return;
	int player=2;
	int winner=0;
	int num_vacancy=0;
	int position=ct;
	int ind_vacancy=0;
	int iter=1;
	while(true)
	{
		/*
		swprintf(buff,100,L"in simulation %d %d",top[position]-1,position);
		MessageBox(NULL, buff, L"Msg title", MB_OK | MB_ICONQUESTION);
		*/
		simulate[top[position]-1][position]=player;
		top[position]--;
		
		winner=checkWins(simulate, M, N);
		
		if(winner!=0)
			break;
		//检查余位
		num_vacancy=0;
	
		for(int i=0; i<N; i++){
			if(top[i]!=0){
				num_vacancy++;
				
			}
		}
		if(num_vacancy==0)
			break;
		//进入下一次循环
		//寻找下一个有效的位置
		position=rand()%num_vacancy;
		ind_vacancy=0;
		for(int i=0; i<N;i++){
			if(top[i]!=0){
				if(ind_vacancy==position){
					position=i;
					break;
				}
				else
					ind_vacancy++;

			}
		}
		player=1+player%2;
		iter++;
	}
	if(winner==0){
		wins[ct]++;
		play[ct]=play[ct]+2;
	}
	else{
		play[ct]++;
		if(winner==2)
			wins[ct]++;
	}
	return;
}

int checkWins(int** simulate, int M, int N){
	int player=0;
	int i_new,j_new;
	for(int i=0; i<M; i++)
		for(int j=0; j<N; j++){
			if(simulate[i][j]==0)
				continue;
			else{
			
				player=simulate[i][j];
		
				for(int k=0;k<4;k++){
					i_new=i;
					j_new=j+k;
					if(i_new>=0 && i_new<M && j_new>=0 && j_new<N && simulate[i_new][j_new]==player){
						if(k==3)
							return player;
					}
					else{
						break;
					}
				}
				for(int k=0;k<4;k++){
					i_new=i+k;
					j_new=j;
					if(i_new>=0 && i_new<M && j_new>=0 && j_new<N && simulate[i_new][j_new]==player){
						if(k==3)
							return player;
					}
					else{
						break;
					}
				}
				for(int k=0;k<4;k++){
					i_new=i+k;
					j_new=j+k;
					if(i_new>=0 && i_new<M && j_new>=0 && j_new<N && simulate[i_new][j_new]==player){
						if(k==3)
							return player;
					}
					else{
						break;
					}
				}
				for(int k=0;k<4;k++){
					i_new=i-k;
					j_new=j+k;
					if(i_new>=0 && i_new<M && j_new>=0 && j_new<N && simulate[i_new][j_new]==player){
						if(k==3)
							return player;
					}
					else{
						break;
					}
				}
				
			}
		}
		return 0;
}
