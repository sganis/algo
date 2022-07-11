// Dual Momentum V1.0 trading, implementation by Hredot ///////////////////
// Disclaimer: Use at your own risk!                    ///////////////////
// You alone are responsible for your gains or losses.  ///////////////////

// Let's have space for up to 250 assets, just in case
#define NUMBEROFASSETS  255

var assetWeight[NUMBEROFASSETS];// Will hold the index and performance stat of up to 1000 assets
int assetSorted[NUMBEROFASSETS];// Will hold the index of assetWeight sorted in decreasing order of performance
vars Price[NUMBEROFASSETS];     // Price history for each asset
int assetNum;             // Will contain the actual number of assets
int assetPlusNum;         // Will contain the actual number of assets with positive return

function bubbleSort(){    // sorts assetSorted to contain assetWeight indices in decreasing weight order
	int i,s,tmp;
	s=assetNum-1;
	while(s>0){
		for(i=0;i<s;i++){
			if(assetWeight[assetSorted[i]] < assetWeight[assetSorted[i+1]]){ 
				tmp=assetSorted[i];
				assetSorted[i]=assetSorted[i+1];
				assetSorted[i+1]=tmp;
			}
		}
		s--;
	}
}

function run()
{
	set(PARAMETERS,LOGFILE+PRELOAD);
	BarPeriod = 1440;	// 1 day bars
	LookBack = 24*20;	// two years lookback period, just in case
	StartDate = 2010;
	//EndDate = 2021;
	Capital = slider(1, 100000, 0, 100000, "Capital", "How much money you start trading with.");
	int myLookBack = 9;   // 9 month lookback
	int myDaysUpdate = 15; //optimize(7,7,35,7); // after how many days to update trades
	int myOpenMax = 10;
	int mySROC = 40; //optimize(10,10,40,5); 
	
	if(is(INITRUN)) {
		assetList("History\\AssetsNasdaq100.csv"); // load asset list
	}

   // Implement absolute momentum:
   // portfolio loop to determine performance of each asset
	assetNum=0;
	assetPlusNum=0;
	
	while(loop(Assets))
	{
		asset(Loop1);
		//assetHistory(Loop1, FROM_STOOQ);
		assetHistory(Loop1, FROM_YAHOO);
		Price[assetNum]=series(price());
		assetWeight[assetNum] = SIROC(Price[assetNum],myLookBack*20,myLookBack*10);
	
		if (assetWeight[assetNum] > mySROC) {
			 assetPlusNum++; // count up a wanted asset
			 //printf("\nto BUY: %s %s %f",strdate(YMD,0), Loop1, assetWeight[assetNum]);
		}
		assetSorted[assetNum]=assetNum;
		assetNum++;
	}
	
   // Implement relative momentum:
   // sort AssetSorted to contain assetWeight indices in decreasing weight order	
	bubbleSort();
	
	static int dayspast=0;
	dayspast++;    // increment days count until trade adjustment
	int i;
	// int enterNum = min((assetNum)/2,assetPlusNum); // Participate in at most 1/2 of winning assets
	int enterNum = min(myOpenMax, assetPlusNum); // Participate in at most 10 assets
	
	if(dayspast > myDaysUpdate) {	          // update trades only once a time period myDaysUpdate
		for(i=enterNum; i < assetNum; i++) {   // exit unwanted assets
			asset(Assets[assetSorted[i]]);
			
			//for(open_trades) {
			////	if(0 == strcmp(Asset,Assets[assetSorted[i]]))
			//		printf("\nSELL: %s %s %f",strdate(YMD,0), Assets[assetSorted[i]], assetWeight[i]);
			//}
			
			
			exitLong();
		}

		for(i=0; i < enterNum; i++) {  // enter wanted assets, each at the appropriate weight
			asset(Assets[assetSorted[i]]);
			//printf("\nequity-margin=%f total=%f", Equity-MarginVal, Capital+WinTotal-LossTotal);
			// Lots=(Capital+WinTotal-LossTotal)*Leverage/(4*enterNum*price()*LotAmount);
			// drawdowns with this strategy are miniscule, therefore we reinvest all
			// use Capital*sqrt(1 + (WinTotal-LossTotal)/Capital) 
			// instead of (Capital+WinTotal-LossTotal) if you feel unsafe
			if(NumOpenLong < 3 
				&& NumOpenTotal < myOpenMax 
				&& (Equity - MarginVal)/(Capital+WinTotal-LossTotal) > 0.33 ) {
				Stop=2*ATR(24*20); // 2 years average true range
				
				Margin = Capital * sqrt(1 + (WinTotal-LossTotal)/Capital) / myOpenMax;
				//Margin = (Capital+WinTotal-LossTotal) / myOpenMax;
				//Margin = 0.05 * (Capital+WinTotal-LossTotal);
				
				printf("\nBUY : %s %s Open: %d/%d, position=%f",
					strdate(YMD,0),Assets[assetSorted[i]], NumOpenLong, NumOpenTotal, Margin);
				
				enterLong();
				
			}
		}
		dayspast=0; // reset the day counter after the trades
	}

	asset(Assets[0]);

   //Plot results:	
	PlotWidth = 600;
	PlotHeight1 = 300;	
	PlotHeight2 = 300;
	plot("$ Balance",Balance,NEW,BLUE);
	plot("Equity",Equity,0,GREEN);
	plot("BalanceInTrades",MarginVal,0,RED);
	plot("Open Trades",NumOpenTotal,NEW,BLUE);

}