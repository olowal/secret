#include "SecMath.h"

int32 USecMathStatics::IncrementIndex(int32 IndexCurrent, int32 IndexMax)
{
	return (IndexCurrent + 1) >= IndexMax ? 0 : IndexCurrent + 1;
}
