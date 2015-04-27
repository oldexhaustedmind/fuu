require 'SDK'

function GetOveralEval(EvalValue)

	if EvalValue == UE_RESULT_FILE_OK then
		return "File is OK!"
	elseif EvalValue == UE_RESULT_FILE_INVALID_BUT_FIXABLE then
		return "File is invalid but fixable!"
	elseif EvalValue == UE_RESULT_FILE_INVALID_AND_NON_FIXABLE then
		return "File is invalid and non fixable!"
	elseif EvalValue == E_RESULT_FILE_INVALID_FORMAT then
		return "File is invalid format!"
	else
		return "INVALID STRUCTURE VALUE"
	end
end

function GetFieldEval(EvalValue)

	if EvalValue == UE_FIELD_OK then
		return "OK"
	elseif EvalValue == UE_FIELD_BROKEN_NON_FIXABLE then
		return "FIXABLE"
	elseif EvalValue == UE_FIELD_BROKEN_NON_CRITICAL then
		return "NON CRITICAL"
	elseif EvalValue == UE_FIELD_BROKEN_FIXABLE_FOR_STATIC_USE then
		return "FIXABLE:STATIC"
	elseif EvalValue == UE_FIELD_BROKEN_BUT_CAN_BE_EMULATED then
		return "BROKEN:EMULATE"
	elseif EvalValue == UE_FILED_FIXABLE_NON_CRITICAL then
		return "FIXABLE:OPTIONAL"
	elseif EvalValue == UE_FILED_FIXABLE_CRITICAL then
		return "FIXABLE:CRITICAL"
	elseif EvalValue == UE_FIELD_NOT_PRESET then
		return "NOT PRESENT"
	elseif EvalValue == UE_FIELD_NOT_PRESET_WARNING then
		return "NOT PRESENT:WARNING"
	else
		return "INVALID STRUCTURE VALUE"
	end
end

function GetBooleanEval(EvalValue)

	if EvalValue == 1 then
		return "True"
	else
		return "False"
	end
end


fileStatus = FILE_STATUS_INFO:new()

fileInfo = TE_IsPE32FileValidEx(arg[1], UE_DEPTH_DEEP, fileStatus())
if not(fileInfo == nil) then
	print(" [+] Selected file is valid!")
else
	print(" [-] Selected file is NOT valid!");
	print(" [-] Selected file OveralEvaluation: " .. GetOveralEval(fileStatus.OveralEvaluation))
	print("     [x] FileIsDLL: " .. GetBooleanEval(fileStatus.FileIsDLL))
	print("     [x] FileIsConsole: " .. GetBooleanEval(fileStatus.FileIsConsole))
	print("     [x] FileIs64Bit: " .. GetBooleanEval(fileStatus.FileIs64Bit))
	print("     [x] EvaluationTerminatedByException: " .. GetBooleanEval(fileStatus.EvaluationTerminatedByException))
	print("     [x] MissingDependencies: " .. GetBooleanEval(fileStatus.MissingDependencies))
	print("     [x] MissingDeclaredAPIs: " .. GetBooleanEval(fileStatus.MissingDeclaredAPIs))
	print("     [-] File details: ")
	print("         [x] SignatureMZ: " .. GetFieldEval(fileStatus.SignatureMZ))
	print("         [x] SignaturePE: " .. GetFieldEval(fileStatus.SignaturePE))
	print("         [x] EntryPoint: " .. GetFieldEval(fileStatus.EntryPoint))
	print("         [x] ImageBase: " .. GetFieldEval(fileStatus.ImageBase))
	print("         [x] SizeOfImage: " .. GetFieldEval(fileStatus.SizeOfImage))
	print("         [x] FileAlignment: " .. GetFieldEval(fileStatus.FileAlignment))
	print("         [x] SectionAlignment: " .. GetFieldEval(fileStatus.SectionAlignment))
	print("         [x] ExportTable: " .. GetFieldEval(fileStatus.ExportTable))
	print("         [x] RelocationTable: " .. GetFieldEval(fileStatus.RelocationTable))
	print("         [x] ImportTable: " .. GetFieldEval(fileStatus.ImportTable))
	print("         [x] ImportTableSection: " .. GetFieldEval(fileStatus.ImportTableSection))
	print("         [x] ImportTableData: " .. GetFieldEval(fileStatus.ImportTableData))
	print("         [x] IATTable: " .. GetFieldEval(fileStatus.IATTable))
	print("         [x] TLSTable: " .. GetFieldEval(fileStatus.TLSTable))
	print("         [x] LoadConfigTable: " .. GetFieldEval(fileStatus.LoadConfigTable))
	print("         [x] BoundImportTable: " .. GetFieldEval(fileStatus.BoundImportTable))
	print("         [x] COMHeaderTable: " .. GetFieldEval(fileStatus.COMHeaderTable))
	print("         [x] ResourceTable: " .. GetFieldEval(fileStatus.ResourceTable))
	print("         [x] ResourceData: " .. GetFieldEval(fileStatus.ResourceData))
	print("         [x] SectionTable: " .. GetFieldEval(fileStatus.SectionTable))
end
