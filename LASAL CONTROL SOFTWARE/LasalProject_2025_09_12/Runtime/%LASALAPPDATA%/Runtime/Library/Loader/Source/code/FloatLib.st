// This file contains functions for floating-point arithmetic, which can be used with 
// FPU or emulation (recommended by IEEE 754).

#include "..\include\FloatDefs.h"

// The function returns TRUE, if the value has a negative sign.

FUNCTION GLOBAL Real_IsSigned
VAR_INPUT
  Value   : REAL;
END_VAR
VAR_OUTPUT
  bResult : BOOL;
END_VAR
VAR
  udValue : UDINT;
END_VAR

  // cast float to udint to avoid floating operations
  udValue := Value$UDINT;
  
  if ( udValue AND MASK_SIGN ) then
    bResult := TRUE;
  else
    bResult := FALSE;
  end_if;
  
END_FUNCTION

// The function returns TRUE, if the value is normal (not zero, subnormal, infinity, or NaN).

FUNCTION GLOBAL Real_IsNormal
VAR_INPUT
  Value   : REAL;
END_VAR
VAR_OUTPUT
  bResult   : BOOL;
END_VAR
VAR
  udValue   : UDINT;
END_VAR

  udValue := Value$UDINT;

  // exponent > 0 and < 255
  if ( ( ( udValue AND MASK_EXPONENT ) <> 0 )
     & ( ( udValue AND MASK_EXPONENT ) <> MASK_EXPONENT ) ) then
    bResult := TRUE;
  else
    bResult := FALSE;
  end_if;
  
END_FUNCTION

// The function returns TRUE, if the value is zero, subnormal or normal (not infinity or NaN).

FUNCTION GLOBAL Real_IsFinite
VAR_INPUT
  Value   : REAL;
END_VAR
VAR_OUTPUT
  bResult   : BOOL;
END_VAR
VAR
  udValue   : UDINT;
END_VAR

  udValue := Value$UDINT;

  // exponent < 255
  if ( ( udValue AND MASK_EXPONENT ) <> MASK_EXPONENT ) then
    bResult := TRUE;
  else
    bResult := FALSE;
  end_if;
  
END_FUNCTION

// The function returns TRUE, if the value is positive or negative zero.

FUNCTION GLOBAL Real_IsZero
VAR_INPUT
  Value   : REAL;
END_VAR
VAR_OUTPUT
  bResult   : DINT;
END_VAR
VAR
  udValue   : UDINT;
END_VAR

  udValue := Value$UDINT;

  if ( ( udValue AND NOT(MASK_SIGN) ) = 0 ) then
    bResult := TRUE;
  else
    bResult := FALSE;
  end_if;

END_FUNCTION

// The function returns TRUE, if the value is subnormal.

FUNCTION GLOBAL Real_IsSubnormal
VAR_INPUT
  Value   : REAL;
END_VAR
VAR_OUTPUT
  bResult   : DINT;
END_VAR
VAR
  udValue   : UDINT;
END_VAR

  udValue := Value$UDINT;

  // exponent = 0 and significand <> 0
  if ( ( ( udValue AND MASK_EXPONENT ) = 0 )
     & ( ( udValue AND MASK_SIGNIFICAND ) <> 0 ) ) then
    bResult := TRUE;
  else
    bResult := FALSE;
  end_if;

END_FUNCTION

// The function returns TRUE, if the value is infinite.

FUNCTION GLOBAL Real_IsInfinite
VAR_INPUT
  Value   : REAL;
END_VAR
VAR_OUTPUT
  bResult   : DINT;
END_VAR
VAR
  udValue   : UDINT;
END_VAR

  udValue := Value$UDINT;

  // exponent = 255 and significand = 0
  if ( ( ( udValue AND MASK_EXPONENT ) = MASK_EXPONENT )
     & ( ( udValue AND MASK_SIGNIFICAND ) = 0 ) ) then
    bResult := TRUE;
  else
    bResult := FALSE;
  end_if;

END_FUNCTION

// The function returns TRUE, if the value is a NaN.

FUNCTION GLOBAL Real_IsNaN
VAR_INPUT
  Value   : REAL;
END_VAR
VAR_OUTPUT
  bResult   : DINT;
END_VAR
VAR
  udValue   : UDINT;
END_VAR
  
  udValue := Value$UDINT;

  // exponent = 255 and significand <> 0
  if ( ( ( udValue AND MASK_EXPONENT ) = MASK_EXPONENT )
     & ( ( udValue AND MASK_SIGNIFICAND ) <> 0 ) ) then
    bResult := TRUE;
  else
    bResult := FALSE;
  end_if;

END_FUNCTION

// The function returns TRUE, if the value is a signaling NaN.

FUNCTION GLOBAL Real_IsSignaling
VAR_INPUT
  Value   : REAL;
END_VAR
VAR_OUTPUT
  bResult   : DINT;
END_VAR
VAR
  udValue   : UDINT;
END_VAR
  
  udValue := Value$UDINT;

  // exponent = 255 and significand > 0 and < 16#40_0000
  if ( ( ( udValue AND MASK_EXPONENT ) = MASK_EXPONENT )
     & ( ( udValue AND MASK_SIGNIFICAND ) <> 0 ) 
     & ( ( udValue AND MASK_QUIET_NAN ) = 0 ) ) then
    bResult := TRUE;
  else
    bResult := FALSE;
  end_if;

END_FUNCTION

// The function returns the class, in which the value falls into.

FUNCTION GLOBAL Real_GetClass
VAR_INPUT
  Value   : REAL;
END_VAR
VAR_OUTPUT
  Result   : REAL_CLASS;
END_VAR
VAR
  udValue   : UDINT;
END_VAR
  
  udValue := Value$UDINT;
  
  // infinite or NaN
  if ( ( udValue AND MASK_EXPONENT ) = MASK_EXPONENT ) then
    
    // infinite
    if ( ( udValue AND MASK_SIGNIFICAND ) = 0 ) then
    
      // negative infinite
      if ( udValue AND MASK_SIGN ) then
        Result := REAL_NegativeInfinity;
        
      // positive infinite
      else
        Result := REAL_PositiveInfinity;
      end_if;
      
    // signaling NaN
    elsif ( ( udValue AND MASK_QUIET_NAN ) = 0 ) then
      Result := REAL_SignalingNaN;
    
    // quit NaN
    else
      Result := REAL_QuietNaN;
    end_if;
  
  // normal
  elsif ( ( udValue AND MASK_EXPONENT ) <> 0 ) then
  
    // negative normal
    if ( udValue AND MASK_SIGN ) then
      Result := REAL_NegativeNormal;
        
    // positive normal
    else
      Result := REAL_PositiveNormal;
    end_if;
  
  // subnormal
  elsif ( ( udValue AND MASK_SIGNIFICAND ) <> 0 ) then 
  
    // negative subnormal
    if ( udValue AND MASK_SIGN ) then
      Result := REAL_NegativeSubnormal;
      
    // positive subnormal
    else
      Result := REAL_PositiveSubnormal;
    end_if;
    
  // negative zero
  elsif ( udValue AND MASK_SIGN ) then 
    Result := REAL_NegativeZero;
  // positive zero
  else
    Result := REAL_PositiveZero;
  end_if;
  
END_FUNCTION
