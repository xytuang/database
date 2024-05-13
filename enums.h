enum StatementType {
    SELECT,
    INSERT
};

enum PrepareResult {
    PREPARE_SUCCESS,
    PREPARE_FAILURE
};

enum ExecuteResult {
    EXECUTE_SUCCESS,
    EXECUTE_FAILURE,
    EXECUTE_TABLE_FULL
};
