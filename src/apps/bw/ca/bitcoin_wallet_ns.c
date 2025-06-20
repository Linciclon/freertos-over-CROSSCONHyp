//#include <err.h>
#include <stdio.h>
#include <string.h>
#include <tee_client_api.h>
#warning "Incluído tee_client_api.h"
#include <bw_ta.h>
#include <bitcoin_wallet_ns.h>
#include <stdlib.h>
#include <tx.h>

static TEEC_Result ta_bitcoin_cmd_check_masterkey(TEEC_Session sess, uint32_t pin, uint32_t cmd_id, uint32_t err_origin);
static TEEC_Result ta_bitcoin_cmd_generate_new_masterkey(TEEC_Session sess, uint32_t pin, uint32_t cmd_id, uint32_t err_origin, char* mnemonic);
static TEEC_Result ta_bitcoin_cmd_mnemonic_to_masterkey(TEEC_Session sess, uint32_t pin, uint32_t cmd_id, uint32_t err_origin, char* mnemonic);
static TEEC_Result ta_bitcoin_cmd_erase_masterkey(TEEC_Session sess, uint32_t pin, uint32_t cmd_id, uint32_t err_origin);
static TEEC_Result ta_bitcoin_cmd_issue_transactions(TEEC_Session sess, uint32_t pin, uint32_t cmd_id, uint32_t err_origin, uint32_t i);
static TEEC_Result ta_bitcoin_cmd_get_bitcoin_address(TEEC_Session sess, uint32_t pin, uint32_t cmd_id, uint32_t err_origin, uint32_t account_id);

static void print_uint8(uint8_t* array, uint32_t array_len);

int tee_bitcoin_wallet()
{
	TEEC_Result res;
	TEEC_Context ctx;
	TEEC_Session sess;
	TEEC_UUID uuid = TA_BITCOIN_WALLET_UUID;
	uint32_t err_origin;
	uint32_t cmd_id;
	uint32_t pin;
	uint32_t account_id = 0;
	char mnemonic[MNEMONIC_LENGTH] = "";

	//char *argv[] = { "bitcoin_wallet", "1", "1234", "some_mnemonic_phrase", "1" };
	char *argv[] = { "bitcoin_wallet", "1", "1234"};
	int argc = sizeof(argv) / sizeof(argv[0]);

	printf("Entering in Client Application....\n");

	// other param validation
	if(check_params(argc, argv) == -1){
		help();
		return -1;
	}

	// command validation
	cmd_id = argv[1][0]-'0';

	// pin validation
	if(strlen(argv[2])!=4){
		printf("bitcoin_wallet: Incorrect pin.\n");
		return -1;
	}
	// mnemonic validation
	if(cmd_id==3){
		char* temp = mnemonic;
		const char space = ' ';
		int i;
		for(i=4;i<argc;i++){
			printf("BW caractere: %s\n", argv[i]);
			strncpy(temp, argv[i], strlen(argv[i]));
			temp += strlen(argv[i]);
			if(i!=(argc-1)){
				strncpy(temp, &space, 1);
				temp += 1;
			}
		}
	}

	if(cmd_id==5 || cmd_id==6){
		printf("account_id: %d\n", argv[4][0]);
		account_id = argv[4][0]-'0';
	}

	//printf("Account ID: %d\n", account_id);

	// convert string pin to integer
	pin = (argv[2][0]-'0')*1000 + (argv[2][1]-'0')*100 + (argv[2][2]-'0')*10 +(argv[2][3]-'0');
	
	// Initiliaze context
	res = TEEC_InitializeContext(NULL, &ctx);
	if (res != TEEC_SUCCESS)
		printf( "TEEC_InitializeContext failed with code 0x%x", res);
	
	// Open session with TA
	res = TEEC_OpenSession(&ctx, &sess, &uuid, TEEC_LOGIN_PUBLIC, NULL, NULL, &err_origin);
	if (res != TEEC_SUCCESS)
		//printf( "TEEC_Opensession failed with code 0x%x origin 0x%x", res, err_origin);

	cmd_id = 7;

	switch(cmd_id){
	    case 1:
		ta_bitcoin_cmd_check_masterkey(sess, pin, 1, err_origin);
		break;
	    case 2:
		ta_bitcoin_cmd_generate_new_masterkey(sess, pin, 2, err_origin, mnemonic);
		break;
	    case 3:
		ta_bitcoin_cmd_mnemonic_to_masterkey(sess, pin, 3, err_origin, mnemonic);
		break;
	    case 4:
		ta_bitcoin_cmd_erase_masterkey(sess, pin, 4, err_origin);
		break;
	    case 5:
		ta_bitcoin_cmd_issue_transactions(sess, pin, 5, err_origin, account_id);
		break;
	    case 6:
		ta_bitcoin_cmd_get_bitcoin_address(sess,pin, 6, err_origin, account_id);
		break;
	    case 7:
		//Check creation and check master of masterkey creation
		ta_bitcoin_cmd_check_masterkey(sess, pin, 1, err_origin);
		ta_bitcoin_cmd_generate_new_masterkey(sess, pin, 2, err_origin, mnemonic);
		ta_bitcoin_cmd_check_masterkey(sess, pin, 1, err_origin);
		//ta_bitcoin_cmd_erase_masterkey(sess, pin, 4, err_origin);
		//ta_bitcoin_cmd_check_masterkey(sess, pin, 1, err_origin);
		ta_bitcoin_cmd_issue_transactions(sess, pin, 5, err_origin, account_id);
		//
		//ta_bitcoin_cmd_mnemonic_to_masterkey(sess, pin, 3, err_origin, mnemonic);
		//getchar();
		break;
	    default:
		help();
		break;
	}

	// Close session
	TEEC_CloseSession(&sess);

	TEEC_FinalizeContext(&ctx);

	return 0;
}

static TEEC_Result ta_bitcoin_cmd_check_masterkey(TEEC_Session sess, uint32_t pin, uint32_t cmd_id, uint32_t err_origin){
	TEEC_Operation op;
	TEEC_Result res;

	// Clear the TEEC_Operation struct, same as clearing parameter type
	memset(&op, 0, sizeof(op));

	printf("Check if there's a existing master key...\n");
	// Defining the parameter value sent to TA, the parameter are 
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INOUT, TEEC_VALUE_OUTPUT , TEEC_NONE, TEEC_NONE);
	op.params[0].value.a = pin;
	op.params[0].value.b = 0; // 0---unchecked, 1---correct pin, 2---incorrect pin
	
	printf("Run TEEC_InvokeCommand...\n"); //joao
	res = TEEC_InvokeCommand(&sess, cmd_id, &op, &err_origin);
	if (res != TEEC_SUCCESS)
		printf( "TEEC_InvokeCommand failed with code 0x%x origin 0x%x", res, err_origin);
	if (op.params[0].value.b == 2)
		printf( "Incorrect pin");

	if(op.params[1].value.a == 1)
		printf("Master key exists yeeiiiiii\n");
	else
		printf("None master key exits\n");

	return TEEC_SUCCESS;
}

static TEEC_Result ta_bitcoin_cmd_generate_new_masterkey(TEEC_Session sess, uint32_t pin, uint32_t cmd_id, uint32_t err_origin, char *mnemonic){
	TEEC_Operation op;
	TEEC_Result res;

	// Clear the TEEC_Operation struct, same as clearing parameter type
	memset(&op, 0, sizeof(op));

	printf("Generating new master key...\n");
	// Defining the parameter value sent to TA, the parameter are 
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INOUT, TEEC_MEMREF_TEMP_OUTPUT , TEEC_NONE, TEEC_NONE);
	op.params[0].value.a = pin;
	op.params[0].value.b = 0; // 0---unchecked, 1---correct pin, 2---incorrect pin
	op.params[1].tmpref.buffer = malloc(240);
	op.params[1].tmpref.size = 240;
	
	res = TEEC_InvokeCommand(&sess, cmd_id, &op, &err_origin);
	if (res != TEEC_SUCCESS)
		printf( "TEEC_InvokeCommand failed with code 0x%x origin 0x%x", res, err_origin);
	if (op.params[0].value.b == 2)
		printf( "Incorrect pin");
	if (res == TEEC_SUCCESS){
		printf("Here's your wallet mnemonic!\n");
		printf("*%s*\n", (char*)op.params[1].tmpref.buffer);
		for (uint8_t i = 0; i < strlen(op.params[1].tmpref.buffer); i++)
		{
			mnemonic[i] = *((char*) (op.params[1].tmpref.buffer) + i);
		}
		printf("\n");
		memset(op.params[1].tmpref.buffer, 0, op.params[1].tmpref.size);
	}
	return TEEC_SUCCESS;
}

static TEEC_Result ta_bitcoin_cmd_mnemonic_to_masterkey(TEEC_Session sess, uint32_t pin, uint32_t cmd_id, uint32_t err_origin, char* mnemonic){
	TEEC_Operation op;
	TEEC_Result res;

	// Clear the TEEC_Operation struct, same as clearing parameter type
	memset(&op, 0, sizeof(op));

	printf("Deriving master key from your mnemonic...\n");
	// Defining the parameter value sent to TA, the parameter are 
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INOUT, TEEC_MEMREF_TEMP_INPUT , TEEC_NONE, TEEC_NONE);
	op.params[0].value.a = pin;
	op.params[0].value.b = 0; // 0---unchecked, 1---correct pin, 2---incorrect pin
	op.params[1].tmpref.buffer = mnemonic;
	op.params[1].tmpref.size = strlen(mnemonic);
	
	res = TEEC_InvokeCommand(&sess, cmd_id, &op, &err_origin);
	if (res != TEEC_SUCCESS)
		printf( "TEEC_InvokeCommand failed with code 0x%x origin 0x%x", res, err_origin);
	if (op.params[0].value.b == 2)
		printf( "Incorrect pin");
	if (res == TEEC_SUCCESS){
		printf("Success!\n");
	}
	return TEEC_SUCCESS;
}

static TEEC_Result ta_bitcoin_cmd_erase_masterkey(TEEC_Session sess, uint32_t pin, uint32_t cmd_id, uint32_t err_origin){
	TEEC_Operation op;
	TEEC_Result res;

	// Clear the TEEC_Operation struct, same as clearing parameter type
	memset(&op, 0, sizeof(op));

	printf("Check if there's a existing master key...\n");
	// Defining the parameter value sent to TA, the parameter are 
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INOUT, TEEC_VALUE_OUTPUT , TEEC_NONE, TEEC_NONE);
	op.params[0].value.a = pin;
	op.params[0].value.b = 0; // 0---unchecked, 1---correct pin, 2---incorrect pin
	
	res = TEEC_InvokeCommand(&sess, cmd_id, &op, &err_origin);
	if (res != TEEC_SUCCESS)
		printf( "TEEC_InvokeCommand failed with code 0x%x origin 0x%x", res, err_origin);
	if (op.params[0].value.b == 2)
		printf( "Incorrect pin");

	if(op.params[1].value.a == 1)
		printf("Master key exists, erase success\n");
	else
		printf("None master key exits\n");

	return TEEC_SUCCESS;
}

static TEEC_Result ta_bitcoin_cmd_issue_transactions(TEEC_Session sess, uint32_t pin, uint32_t cmd_id, uint32_t err_origin, uint32_t account_id){

	TEEC_Operation op;
	TEEC_Result res;

	/* We hardcode some previous transaction hashses to use for demo purposes */
	const uint8_t prvh[] = {0xdf,0x0b,0x6a,0xfa,0xf0,0xde,0xcd,0x00,0x27,0x80,0xb2,
							0x91,0x16,0x3c,0xad,0xdb,0x76,0x57,0x5f,0xf5,0x5f,0x62,
							0xb1,0x19,0xa2,0x0f,0xda,0x07,0x2f,0x2c,0x47,0xb9};

	const uint8_t tmpsig[] = {OP_DUP,OP_HASH160,0x46,0x2c,0x22,0x2c,0xba,0xac,0x40,
							  0x9f,0x77,0x3b,0x4c,0x0e,0xca,0x99,0x19,0x04,0xc2,0xbb,
							  0x20,0x73,OP_EQUALVERIFY};

	/* Transaction value 0.005555 BTC or 0x15b3 hex */
	//const uint8_t txval[] = {0x62, 0x64, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};
	const uint8_t txval[] = {0xB3, 0x15, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	uint8_t *out;

	INTX intx = NULL;
	OUTTX outtx = NULL;
	TX tx = NULL;
	TXBUF buf = NULL;

	intx = new_intx(prvh, tmpsig);
	outtx = new_outtx(txval, tmpsig);
	tx = new_tx(intx, outtx);
	buf = new_txbuf();
	out = malloc(72); //preciso dele mais tarde

	//print_tx(tx);
	serialize_tx(tx, buf);
	//print_ser_bytes(buf);
	printf("Sending transaction for signing.\n");

	// Clear the TEEC_Operation struct, same as clearing parameter type
	memset(&op, 0, sizeof(op));

	printf("Attempt to issue transactions from %d child account...\n", account_id);
	// Defining the parameter value sent to TA, the parameter are 
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INOUT, TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_OUTPUT, TEEC_VALUE_INPUT);
	op.params[0].value.a = pin;
	op.params[0].value.b = 0; // 0---unchecked, 1---correct pin, 2---incorrect pin
	op.params[1].tmpref.buffer = get_txbuf_data(buf);
	op.params[1].tmpref.size = get_txbuf_next(buf);
	op.params[2].tmpref.buffer = out;
	op.params[2].tmpref.size = 72;
	op.params[3].value.a = account_id;

	res = TEEC_InvokeCommand(&sess, cmd_id, &op, &err_origin);
	if (res != TEEC_SUCCESS)
		//printf( "TEEC_InvokeCommand failed with code 0x%x origin 0x%x", res, err_origin);
	if (op.params[0].value.b == 2)
		//printf( "Incorrect pin");
	
	free_tx(tx);
	free_txbuf(buf);

	printf("Retrieved signed transaction\n");
	for (uint32_t i = 0; i < op.params[2].tmpref.size; i++) {
		printf("%02x", out[i]);
	}
	printf("\n");

	return res;
}

static TEEC_Result ta_bitcoin_cmd_get_bitcoin_address(TEEC_Session sess, uint32_t pin, uint32_t cmd_id, uint32_t err_origin, uint32_t account_id){
	TEEC_Operation op;
	TEEC_Result res;

	// Clear the TEEC_Operation struct, same as clearing parameter type
	memset(&op, 0, sizeof(op));

	printf("Getting bitcoin address...\n");
	// Defining the parameter value sent to TA, the parameter are 
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INOUT, TEEC_VALUE_INPUT, TEEC_MEMREF_TEMP_OUTPUT, TEEC_NONE);
	op.params[0].value.a = pin;
	op.params[0].value.b = 0; // 0---unchecked, 1---correct pin, 2---incorrect pin
	op.params[1].value.a = account_id;
	op.params[2].tmpref.buffer = malloc(25);
	op.params[2].tmpref.size = 25;
	
	res = TEEC_InvokeCommand(&sess, cmd_id, &op, &err_origin);
	if (res != TEEC_SUCCESS)
		//printf( "TEEC_InvokeCommand failed with code 0x%x origin 0x%x", res, err_origin);
	if (op.params[0].value.b == 2)
		//printf( "Incorrect pin");
	
	print_uint8(op.params[2].tmpref.buffer,25);
	return TEEC_SUCCESS;
}

int help(){

	printf("Usage: bitcoin_wallet [OPTION]... [PIN]... -a [MNEMONIC](optional)\n");
	printf("OPTIONS:\n");
	printf("\t1 --- Check if there's a master key\n");
	printf("\t2 --- Generate master key\n");// will return the mnemonic
	printf("\t3 --- from mnemonic to master key, only support 12 mnemonics\n");
	printf("\t4 --- Erase master key\n");
	printf("\t5 --- Sign transaction with ith child key\n");
	printf("\t6 --- Get bitcoin address\n");

	return 0;
}

int check_params(int argc, char *argv[]){
	// cmd_id check
	if(strlen(argv[1])!=1 || argv[1][0]<'0' || argv[1][0]>'7'){
		return -1;
	}

	// option 3
	if(argv[1][0]=='3'){
		printf("argc: %d,argv[3]: %s\n", argc, argv[3]);
		if((argc==28 && !strcmp(argv[3],"-a")) || (argc==16 && !strcmp(argv[3],"-a")))
			return 1;
		else
			return -1;
	}

	//option 5 --- only support 9 accounts
	if(argv[1][0]=='5'){
		if(argc==5 && !strcmp(argv[3],"-a") && strlen(argv[4])==1 && argv[4]>0 && argv[4]<9)
			return 1;
		else
			return -1;
	}

	//option 6
	if(argv[1][0]=='6'){
		// printf("argv[4]: %s\n",argv[4]);
		// printf("argv[4]: %d\n",argv[4][0]);
		// printf("strlen(argv[4]: %d\n",strlen(argv[4]));
		// printf("zero letter: %d\n",'0');
		// printf("prev zero letter: %d\n",('0'-1));  
		// printf("argc: %d\n",argc);  	
		if(argc==5 && !strcmp(argv[3],"-a") && strlen(argv[4])==1 && argv[4][0]>('0'-1) && argv[4][0]<'9')
			return 1;
		else
			return -1;
	}

	return 0;
}

static void print_uint8(uint8_t* array, uint32_t array_len){
	uint32_t i;
	for(i=0; i<array_len; i++){
		printf("%x", array[i]);
	}
	printf("\n");
}