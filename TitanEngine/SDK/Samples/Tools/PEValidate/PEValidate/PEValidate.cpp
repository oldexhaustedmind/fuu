// PEValidate.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "windows.h"
#include "SDK\SDK.h"

char* GetOveralEval(int EvalValue){

	if(EvalValue == UE_RESULT_FILE_OK){
		return("File is OK!");
	}else if(EvalValue == UE_RESULT_FILE_INVALID_BUT_FIXABLE){
		return("File is invalid but fixable!");
	}else if(EvalValue == UE_RESULT_FILE_INVALID_AND_NON_FIXABLE){
		return("File is invalid and non fixable!");
	}else if(EvalValue == UE_RESULT_FILE_INVALID_FORMAT){
		return("File is invalid format!");
	}else{
		return("INVALID STRUCTURE VALUE");
	}
}

char* GetFieldEval(int EvalValue){

	if(EvalValue == UE_FIELD_OK){
		return("OK");
	}else if(EvalValue == UE_FIELD_BROKEN_NON_FIXABLE){
		return("FIXABLE");
	}else if(EvalValue == UE_FIELD_BROKEN_NON_CRITICAL){
		return("NON CRITICAL");
	}else if(EvalValue == UE_FIELD_BROKEN_FIXABLE_FOR_STATIC_USE){
		return("FIXABLE:STATIC");
	}else if(EvalValue == UE_FIELD_BROKEN_BUT_CAN_BE_EMULATED){
		return("BROKEN:EMULATE");
	}else if(EvalValue == UE_FILED_FIXABLE_NON_CRITICAL){
		return("FIXABLE:OPTIONAL");
	}else if(EvalValue == UE_FILED_FIXABLE_CRITICAL){
		return("FIXABLE:CRITICAL");
	}else if(EvalValue == UE_FIELD_NOT_PRESET){
		return("NOT PRESENT");
	}else if(EvalValue == UE_FIELD_NOT_PRESET_WARNING){
		return("NOT PRESENT:WARNING");
	}else{
		return("INVALID STRUCTURE VALUE");
	}
}

char* GetBooleanEval(int EvalValue){

	if(EvalValue == 1){
		return("True");
	}else{
		return("False");
	}
}

int _tmain(int argc, _TCHAR* argv[]){

	FILE_STATUS_INFO fileStatus = {};

	printf("PEValidator 1.0\r\nReversingLabs Corporation - www.reversinglabs.com\r\n\r\n");
	if(argc > 1){
		if(IsPE32FileValidExW(argv[1], UE_DEPTH_DEEP, &fileStatus)){
			printf(" [+] Selected file is valid!\r\n");
		}else{
			printf(" [-] Selected file is NOT valid!\r\n");
			printf(" [-] Selected file OveralEvaluation: %s\r\n", GetOveralEval(fileStatus.OveralEvaluation));
			printf("     [x] FileIsDLL: %s\r\n", GetBooleanEval(fileStatus.FileIsDLL));
			printf("     [x] FileIsConsole: %s\r\n", GetBooleanEval(fileStatus.FileIsConsole));
			printf("     [x] FileIs64Bit: %s\r\n", GetBooleanEval(fileStatus.FileIs64Bit));
			printf("     [x] EvaluationTerminatedByException: %s\r\n", GetBooleanEval(fileStatus.EvaluationTerminatedByException));
			printf("     [x] MissingDependencies: %s\r\n", GetBooleanEval(fileStatus.MissingDependencies));
			printf("     [x] MissingDeclaredAPIs: %s\r\n", GetBooleanEval(fileStatus.MissingDeclaredAPIs));
			printf("     [-] File details:\r\n");
			printf("         [x] SignatureMZ: %s\r\n", GetFieldEval(fileStatus.SignatureMZ));
			printf("         [x] SignaturePE: %s\r\n", GetFieldEval(fileStatus.SignaturePE));
			printf("         [x] EntryPoint: %s\r\n", GetFieldEval(fileStatus.EntryPoint));
			printf("         [x] ImageBase: %s\r\n", GetFieldEval(fileStatus.ImageBase));
			printf("         [x] SizeOfImage: %s\r\n", GetFieldEval(fileStatus.SizeOfImage));
			printf("         [x] FileAlignment: %s\r\n", GetFieldEval(fileStatus.FileAlignment));
			printf("         [x] SectionAlignment: %s\r\n", GetFieldEval(fileStatus.SectionAlignment));
			printf("         [x] ExportTable: %s\r\n", GetFieldEval(fileStatus.ExportTable));
			printf("         [x] RelocationTable: %s\r\n", GetFieldEval(fileStatus.RelocationTable));
			printf("         [x] ImportTable: %s\r\n", GetFieldEval(fileStatus.ImportTable));
			printf("         [x] ImportTableSection: %s\r\n", GetFieldEval(fileStatus.ImportTableSection));
			printf("         [x] ImportTableData: %s\r\n", GetFieldEval(fileStatus.ImportTableData));
			printf("         [x] IATTable: %s\r\n", GetFieldEval(fileStatus.IATTable));
			printf("         [x] TLSTable: %s\r\n", GetFieldEval(fileStatus.TLSTable));
			printf("         [x] LoadConfigTable: %s\r\n", GetFieldEval(fileStatus.LoadConfigTable));
			printf("         [x] BoundImportTable: %s\r\n", GetFieldEval(fileStatus.BoundImportTable));
			printf("         [x] COMHeaderTable: %s\r\n", GetFieldEval(fileStatus.COMHeaderTable));
			printf("         [x] ResourceTable: %s\r\n", GetFieldEval(fileStatus.ResourceTable));
			printf("         [x] ResourceData: %s\r\n", GetFieldEval(fileStatus.ResourceData));
			printf("         [x] SectionTable: %s\r\n", GetFieldEval(fileStatus.SectionTable));
			printf("\r\n");
		}
	}else{
		printf(" Usage: PEValidate inputFileName.ext\r\n");
	}
	return 0;
}

