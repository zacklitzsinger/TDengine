#include "sclfunc.h"
#include <common/tdatablock.h>
#include "sclInt.h"
#include "sclvector.h"

static void assignBasicParaInfo(struct SScalarParam* dst, const struct SScalarParam* src) {
//  dst->type = src->type;
//  dst->bytes = src->bytes;
//  dst->num = src->num;
}

/** Math functions **/
int32_t absFunction(SScalarParam *pInput, int32_t inputNum, SScalarParam *pOutput) {
  SColumnInfoData *pInputData  = pInput->columnData;
  SColumnInfoData *pOutputData = pOutput->columnData;

  int32_t type = GET_PARAM_TYPE(pInput);
  if (!IS_NUMERIC_TYPE(type)) {
    return TSDB_CODE_FAILED;
  }

  switch (type) {
    case TSDB_DATA_TYPE_FLOAT: {
      float *in  = (float *)pInputData->pData;
      float *out = (float *)pOutputData->pData;
      for (int32_t i = 0; i < pInput->numOfRows; ++i) {
        if (colDataIsNull_f(pInputData->nullbitmap, i)) {
          colDataSetNull_f(pOutputData->nullbitmap, i);
          continue;
        }
        out[i] = (in[i] > 0)? in[i] : -in[i];
      }
      break;
    }

    case TSDB_DATA_TYPE_DOUBLE: {
      double *in  = (double *)pInputData->pData;
      double *out = (double *)pOutputData->pData;
      for (int32_t i = 0; i < pInput->numOfRows; ++i) {
        if (colDataIsNull_f(pInputData->nullbitmap, i)) {
          colDataSetNull_f(pOutputData->nullbitmap, i);
          continue;
        }
        out[i] = (in[i] > 0)? in[i] : -in[i];
      }
      break;
    }

    case TSDB_DATA_TYPE_TINYINT: {
      int8_t *in  = (int8_t *)pInputData->pData;
      int8_t *out = (int8_t *)pOutputData->pData;
      for (int32_t i = 0; i < pInput->numOfRows; ++i) {
        if (colDataIsNull_f(pInputData->nullbitmap, i)) {
          colDataSetNull_f(pOutputData->nullbitmap, i);
          continue;
        }
        out[i] = (in[i] > 0)? in[i] : -in[i];
      }
      break;
    }

    case TSDB_DATA_TYPE_SMALLINT: {
      int16_t *in  = (int16_t *)pInputData->pData;
      int16_t *out = (int16_t *)pOutputData->pData;
      for (int32_t i = 0; i < pInput->numOfRows; ++i) {
        if (colDataIsNull_f(pInputData->nullbitmap, i)) {
          colDataSetNull_f(pOutputData->nullbitmap, i);
          continue;
        }
        out[i] = (in[i] > 0)? in[i] : -in[i];
      }
      break;
    }

    case TSDB_DATA_TYPE_INT: {
      int32_t *in  = (int32_t *)pInputData->pData;
      int32_t *out = (int32_t *)pOutputData->pData;
      for (int32_t i = 0; i < pInput->numOfRows; ++i) {
        if (colDataIsNull_f(pInputData->nullbitmap, i)) {
          colDataSetNull_f(pOutputData->nullbitmap, i);
          continue;
        }
        out[i] = (in[i] > 0)? in[i] : -in[i];
      }
      break;
    }

    case TSDB_DATA_TYPE_BIGINT: {
      int64_t *in  = (int64_t *)pInputData->pData;
      int64_t *out = (int64_t *)pOutputData->pData;
      for (int32_t i = 0; i < pInput->numOfRows; ++i) {
        if (colDataIsNull_f(pInputData->nullbitmap, i)) {
          colDataSetNull_f(pOutputData->nullbitmap, i);
          continue;
        }
        out[i] = (in[i] > 0)? in[i] : -in[i];
      }
      break;
    }

    default: {
      colDataAssign(pOutputData, pInputData, pInput->numOfRows);
    }
  }

  pOutput->numOfRows = pInput->numOfRows;
  return TSDB_CODE_SUCCESS;
}

int32_t logFunction(SScalarParam *pInput, int32_t inputNum, SScalarParam *pOutput) {
#if 0
  if (inputNum != 2 || !IS_NUMERIC_TYPE(pInput[0].type) || !IS_NUMERIC_TYPE(pInput[1].type)) {
    return TSDB_CODE_FAILED;
  }

  char **input = NULL, *output = NULL;
  bool hasNullInput = false;
  input = taosMemoryCalloc(inputNum, sizeof(char *));
  for (int32_t i = 0; i < pOutput->num; ++i) {
    for (int32_t j = 0; j < inputNum; ++j) {
      if (pInput[j].num == 1) {
        input[j] = pInput[j].data;
      } else {
        input[j] = pInput[j].data + i * pInput[j].bytes;
      }
      if (isNull(input[j], pInput[j].type)) {
        hasNullInput = true;
        break;
      }
    }
    output = pOutput->data + i * pOutput->bytes;

    if (hasNullInput) {
      setNull(output, pOutput->type, pOutput->bytes);
      continue;
    }

    double base;
    GET_TYPED_DATA(base, double, pInput[1].type, input[1]);
    double v;
    GET_TYPED_DATA(v, double, pInput[0].type, input[0]);
    double result = log(v) / log(base);
    SET_TYPED_DATA(output, pOutput->type, result);
  }

  taosMemoryFree(input);
#endif

  return TSDB_CODE_SUCCESS;
}

int32_t powFunction(SScalarParam *pInput, int32_t inputNum, SScalarParam *pOutput) {
#if 0
  if (inputNum != 2 || !IS_NUMERIC_TYPE(pInput[0].type) || !IS_NUMERIC_TYPE(pInput[1].type)) {
    return TSDB_CODE_FAILED;
  }

  pOutput->type = TSDB_DATA_TYPE_DOUBLE;
  pOutput->bytes = tDataTypes[TSDB_DATA_TYPE_DOUBLE].bytes;

  char **input = NULL, *output = NULL;
  bool hasNullInput = false;
  input = taosMemoryCalloc(inputNum, sizeof(char *));
  for (int32_t i = 0; i < pOutput->num; ++i) {
    for (int32_t j = 0; j < inputNum; ++j) {
      if (pInput[j].num == 1) {
        input[j] = pInput[j].data;
      } else {
        input[j] = pInput[j].data + i * pInput[j].bytes;
      }
      if (isNull(input[j], pInput[j].type)) {
        hasNullInput = true;
        break;
      }
    }
    output = pOutput->data + i * pOutput->bytes;

    if (hasNullInput) {
      setNull(output, pOutput->type, pOutput->bytes);
      continue;
    }

    double base;
    GET_TYPED_DATA(base, double, pInput[1].type, input[1]);
    double v;
    GET_TYPED_DATA(v, double, pInput[0].type, input[0]);
    double result = pow(v, base);
    SET_TYPED_DATA(output, pOutput->type, result);
  }

  taosMemoryFree(input);
#endif
  return TSDB_CODE_SUCCESS;
}

typedef float (*_float_fn)(float);
typedef double (*_double_fn)(double);

int32_t doScalarFunctionUnique(SScalarParam *pInput, int32_t inputNum, SScalarParam* pOutput, _double_fn valFn) {
  int32_t type = GET_PARAM_TYPE(pInput);
  if (inputNum != 1 || !IS_NUMERIC_TYPE(type)) {
    return TSDB_CODE_FAILED;
  }

  SColumnInfoData *pInputData = pInput->columnData;
  SColumnInfoData *pOutputData = pOutput->columnData;

  _getDoubleValue_fn_t getValueFn = getVectorDoubleValueFn(type);

  double *out = (double *)pOutputData->pData;

  for (int32_t i = 0; i < pInput->numOfRows; ++i) {
    if (colDataIsNull_f(pInputData->nullbitmap, i)) {
      colDataSetNull_f(pOutputData->nullbitmap, i);
      continue;
    }
    out[i] = valFn(getValueFn(pInputData->pData, i));
  }

  pOutput->numOfRows = pInput->numOfRows;
  return TSDB_CODE_SUCCESS;
}

int32_t doScalarFunction(SScalarParam *pInput, int32_t inputNum, SScalarParam* pOutput, _float_fn f1, _double_fn d1) {
  int32_t type = GET_PARAM_TYPE(pInput);
  if (inputNum != 1 || !IS_NUMERIC_TYPE(type)) {
    return TSDB_CODE_FAILED;
  }

  SColumnInfoData *pInputData  = pInput->columnData;
  SColumnInfoData *pOutputData = pOutput->columnData;

  switch (type) {
    case TSDB_DATA_TYPE_FLOAT: {
      float *in  = (float *)pInputData->pData;
      float *out = (float *)pOutputData->pData;

      for (int32_t i = 0; i < pInput->numOfRows; ++i) {
        if (colDataIsNull_f(pInputData->nullbitmap, i)) {
          colDataSetNull_f(pOutputData->nullbitmap, i);
          continue;
        }
        out[i] = f1(in[i]);
      }
      break;
    }

    case TSDB_DATA_TYPE_DOUBLE: {
      double *in  = (double *)pInputData->pData;
      double *out = (double *)pOutputData->pData;

      for (int32_t i = 0; i < pInput->numOfRows; ++i) {
        if (colDataIsNull_f(pInputData->nullbitmap, i)) {
          colDataSetNull_f(pOutputData->nullbitmap, i);
          continue;
        }
        out[i] = d1(in[i]);
      }
      break;
    }

    default: {
      colDataAssign(pOutputData, pInputData, pInput->numOfRows);
    }
  }

  pOutput->numOfRows = pInput->numOfRows;
  return TSDB_CODE_SUCCESS;
}

int32_t atanFunction(SScalarParam *pInput, int32_t inputNum, SScalarParam *pOutput) {
  return doScalarFunctionUnique(pInput, inputNum, pOutput, atan);
}

int32_t sinFunction(SScalarParam *pInput, int32_t inputNum, SScalarParam *pOutput) {
  return doScalarFunctionUnique(pInput, inputNum, pOutput, sin);
}

int32_t cosFunction(SScalarParam *pInput, int32_t inputNum, SScalarParam *pOutput) {
  return doScalarFunctionUnique(pInput, inputNum, pOutput, cos);
}

int32_t tanFunction(SScalarParam *pInput, int32_t inputNum, SScalarParam *pOutput) {
  return doScalarFunctionUnique(pInput, inputNum, pOutput, tan);
}

int32_t asinFunction(SScalarParam *pInput, int32_t inputNum, SScalarParam *pOutput) {
  return doScalarFunctionUnique(pInput, inputNum, pOutput, asin);
}

int32_t acosFunction(SScalarParam *pInput, int32_t inputNum, SScalarParam *pOutput) {
  return doScalarFunctionUnique(pInput, inputNum, pOutput, acos);
}

int32_t sqrtFunction(SScalarParam *pInput, int32_t inputNum, SScalarParam *pOutput) {
  return doScalarFunctionUnique(pInput, inputNum, pOutput, sqrt);
}

int32_t ceilFunction(SScalarParam *pInput, int32_t inputNum, SScalarParam *pOutput) {
  return doScalarFunction(pInput, inputNum, pOutput, ceilf, ceil);
}

int32_t floorFunction(SScalarParam *pInput, int32_t inputNum, SScalarParam *pOutput) {
  return doScalarFunction(pInput, inputNum, pOutput, floorf, floor);
}

int32_t roundFunction(SScalarParam *pInput, int32_t inputNum, SScalarParam *pOutput) {
  return doScalarFunction(pInput, inputNum, pOutput, roundf, round);
}

static void tlength(SScalarParam* pOutput, size_t numOfInput, const SScalarParam *pLeft) {
  assert(numOfInput == 1);
#if 0
  int64_t* out = (int64_t*) pOutput->data;
  char* s = pLeft->data;

  for(int32_t i = 0; i < pLeft->num; ++i) {
    out[i] = varDataLen(POINTER_SHIFT(s, i * pLeft->bytes));
  }
#endif
}

static void tconcat(SScalarParam* pOutput, size_t numOfInput, const SScalarParam *pLeft) {
  assert(numOfInput > 0);
#if 0
  int32_t rowLen = 0;
  int32_t num = 1;
  for(int32_t i = 0; i < numOfInput; ++i) {
    rowLen += pLeft[i].bytes;

    if (pLeft[i].num > 1) {
      num = pLeft[i].num;
    }
  }

  pOutput->data = taosMemoryRealloc(pOutput->data, rowLen * num);
  assert(pOutput->data);

  char* rstart = pOutput->data;
  for(int32_t i = 0; i < num; ++i) {

    char* s = rstart;
    varDataSetLen(s, 0);
    for (int32_t j = 0; j < numOfInput; ++j) {
      char* p1 = POINTER_SHIFT(pLeft[j].data, i * pLeft[j].bytes);

      memcpy(varDataVal(s) + varDataLen(s), varDataVal(p1), varDataLen(p1));
      varDataLen(s) += varDataLen(p1);
    }

    rstart += rowLen;
  }
#endif
}

static void tltrim(SScalarParam* pOutput, size_t numOfInput, const SScalarParam *pLeft) {

}

static void trtrim(SScalarParam* pOutput, size_t numOfInput, const SScalarParam *pLeft) {

}

static void reverseCopy(char* dest, const char* src, int16_t type, int32_t numOfRows) {
  switch(type) {
    case TSDB_DATA_TYPE_TINYINT:
    case TSDB_DATA_TYPE_UTINYINT:{
      int8_t* p = (int8_t*) dest;
      int8_t* pSrc = (int8_t*) src;

      for(int32_t i = 0; i < numOfRows; ++i) {
        p[i] = pSrc[numOfRows - i - 1];
      }
      return;
    }

    case TSDB_DATA_TYPE_SMALLINT:
    case TSDB_DATA_TYPE_USMALLINT:{
      int16_t* p = (int16_t*) dest;
      int16_t* pSrc = (int16_t*) src;

      for(int32_t i = 0; i < numOfRows; ++i) {
        p[i] = pSrc[numOfRows - i - 1];
      }
      return;
    }
    case TSDB_DATA_TYPE_INT:
    case TSDB_DATA_TYPE_UINT: {
      int32_t* p = (int32_t*) dest;
      int32_t* pSrc = (int32_t*) src;

      for(int32_t i = 0; i < numOfRows; ++i) {
        p[i] = pSrc[numOfRows - i - 1];
      }
      return;
    }
    case TSDB_DATA_TYPE_BIGINT:
    case TSDB_DATA_TYPE_UBIGINT: {
      int64_t* p = (int64_t*) dest;
      int64_t* pSrc = (int64_t*) src;

      for(int32_t i = 0; i < numOfRows; ++i) {
        p[i] = pSrc[numOfRows - i - 1];
      }
      return;
    }
    case TSDB_DATA_TYPE_FLOAT: {
      float* p = (float*) dest;
      float* pSrc = (float*) src;

      for(int32_t i = 0; i < numOfRows; ++i) {
        p[i] = pSrc[numOfRows - i - 1];
      }
      return;
    }
    case TSDB_DATA_TYPE_DOUBLE: {
      double* p = (double*) dest;
      double* pSrc = (double*) src;

      for(int32_t i = 0; i < numOfRows; ++i) {
        p[i] = pSrc[numOfRows - i - 1];
      }
      return;
    }
    default: assert(0);
  }
}

