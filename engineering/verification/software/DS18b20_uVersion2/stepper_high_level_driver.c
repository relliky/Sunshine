#ifndef INTRINS_H
	#include <intrins.h>
	#define INTRINS_H
#endif
	
#ifndef REG52_H
	#include <reg52.h>	
	#define REG52_H
#endif	

#ifndef TIMER_INTERRUPT_H
	#include <timer_interrupt.h>
	#define TIMER_INTERRUPT_H
#endif


#define RUNNING_STATE_NOT_RUN_YET			0
#define RUNNING_STATE_RUNNING				1
#define RUNNING_STATE_FORWARD_RUNNING       0x2
#define RUNNING_STATE_BACKWARD_RUNNING		0x3



bit StateOfStepperForwardRun	= RUNNING_STATE_NOT_RUN_YET;
bit StateOfStepperBackwardRun	= RUNNING_STATE_NOT_RUN_YET;

unsigned char	StepperRunningState = RUNNING_STATE_NOT_RUN_YET;



bit RunStepperForwardForTime(unsigned int RunningTime)
{
	int Finished = NO;
	
	if(StateOfStepperForwardRun == RUNNING_STATE_NOT_RUN_YET)
	{
		StepperForwardStart = YES;
		StateOfStepperForwardRun = RUNNING_STATE_RUNNING;
		TimerSetForStepperLastingInOneMovementInTimer1 = RunningTime;
	}

	if(StateOfStepperForwardRun == RUNNING_STATE_RUNNING)
	{
		if(StepperForwardStop == YES)
		{
			Finished				 = 	YES;
			StepperForwardStop 		 =  NO;
			StateOfStepperForwardRun = 	RUNNING_STATE_NOT_RUN_YET;
		}
	}

	if(Finished == YES) 
	{
		return(FINISHED);
	}else 
		{
			return(CONTINUOUS);			
		}
		
}// end of RunStepperForwardForTime(unsigned int RunningTime)


bit RunStepperBackwardForTime(unsigned int RunningTime)
{
	int Finished = NO;
	
	if(StateOfStepperBackwardRun == RUNNING_STATE_NOT_RUN_YET)
	{
		StepperBackwardStart = YES;
		StateOfStepperBackwardRun = RUNNING_STATE_RUNNING;
		TimerSetForStepperLastingInOneMovementInTimer1 = RunningTime;
	}

	if(StateOfStepperBackwardRun == RUNNING_STATE_RUNNING)
	{
		if(StepperBackwardStop == YES)
		{
			Finished				 	= YES;
			StepperBackwardStop 	 	= NO;
			StateOfStepperBackwardRun 	= RUNNING_STATE_NOT_RUN_YET;
		}
	}

	if(Finished == YES) 
	{
		return(FINISHED);
	}else 
		{
			return(CONTINUOUS);			
		}
		
}// end of RunStepperBackwardForTime(unsigned int RunningTime)


bit RunStepperForwardAndBackwardForTime (unsigned int RunningTimeForTwoCycle)
{
   	unsigned int 	RunningTimeForOneCycle = RunningTimeForTwoCycle / 2;
	bit				Finished			   = NO;
	 			
	
 	if(StepperRunningState == RUNNING_STATE_NOT_RUN_YET)	
		{
			RunStepperForwardForTime(RunningTimeForOneCycle);
			StepperRunningState = RUNNING_STATE_FORWARD_RUNNING;
		}
		
	if(StepperRunningState == RUNNING_STATE_FORWARD_RUNNING)
	{
		if(RunStepperForwardForTime(RunningTimeForOneCycle) == FINISHED) 
				StepperRunningState = RUNNING_STATE_BACKWARD_RUNNING;
	}

	if(StepperRunningState == RUNNING_STATE_BACKWARD_RUNNING)
	{
		if(RunStepperBackwardForTime(RunningTimeForOneCycle) == FINISHED) 
			{
				StepperRunningState = RUNNING_STATE_NOT_RUN_YET;
				Finished = YES;	
			}
	}
 
	if(Finished == YES) 
	{
		return(FINISHED);
	}else 
		{
			return(CONTINUOUS);			
		}	

}// end of RunStepperForwardAndBackwardForTime (unsigned int RunningTimeForTwoCycle)


void PressingButtonsByStepper(unsigned int RunningTimeForTwoCycle, unsigned int *PointerOfNumberOfButtonToPressByStepper)
{
	if(*PointerOfNumberOfButtonToPressByStepper != 0)
	{
		if(RunStepperForwardAndBackwardForTime(RunningTimeForTwoCycle) == FINISHED)
			(*PointerOfNumberOfButtonToPressByStepper)--;
	}

}//  end of PressingButtonsByStepper(unsigned int RunningTimeForTwoCycle, unsigned int *PointerOfNumberOfButtonToPressByStepper)
