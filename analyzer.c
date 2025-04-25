#include <stdio.h>
#include "json_c.c"
#include <string.h>

// 전역변수로 선언
int func_count = 0;
int if_condition_count = 0;

// 파일 읽고 json 객체를 만드는 함수
json_value json_obj() {
   FILE *fp;
   fp = fopen("ast.json", "r");
 
   if (fp == NULL) printf("파일열기 실패\n");
   else printf("파일열기 성공\n");
 
   // length는 794323
   int c = 0;
   int length = 0;
   while((c = fgetc(fp)) != EOF){
     length++;
   }
 
   // 힙에 동적 할당 - 파일의 크기가 크기 때문
   char *fileChar;
   fileChar = (char *)malloc(sizeof(char) * (length+1)); // null 문자를 넣기 위해 +1
 
   fclose(fp);
 
   // 파일을 한번 더 열어서 fileChar에 문자 쓰기
   fp = fopen("ast.json", "r");
 
   if (fp == NULL) printf("파일열기 실패\n");
   else printf("파일열기 성공\n");
 
   // fileChar에 문자 쓰기
   int count = 0;
   while((c = fgetc(fp)) != EOF){
     fileChar[count] = c;
     count++;
   }
   fileChar[count] = '\0'; // 마지막에 NULL 넣기
   fclose(fp);

   json_value json = json_create(fileChar);

   free(fileChar);

   return json;
}

// args가 null이 아닌 경우, 함수 정보 추출(함수 명도 추가했습니다)
void notNull_func(json_value type1) {
  json_value type2 = json_get(type1, "type");
  json_value type3 = json_get(type2, "type");
  int type3_len = json_len(type3);

  // ast.json을 끝까지 파싱할 경우 type->type->type->name 경로와 type->type->name 경로를 구분
  if (type3_len == 3) {
    json_value declname = json_get(type2, "declname");
    printf("함수명: ");
    json_print(declname);
    printf("\n");

    json_value names = json_get(type3, "names");
    printf("함수의 리턴 타입: ");
    json_print(names);
    printf("\n");
  } else {
    json_value declname = json_get(type3, "declname");
    printf("함수명: ");
    json_print(declname);
    printf("\n");

    json_value type4 = json_get(type3, "type");
    json_value names = json_get(type4, "names");
    printf("함수의 리턴 타입: ");
    json_print(names);
    printf("\n");
  }
  
}

// ast.json 파일의 args가 null일 때,
void args_isNull(json_value type1) {
  json_value type2 = json_get(type1, "type");
  json_value type3 = json_get(type2, "type");
  json_value names = json_get(type3, "names");

  json_value declname = json_get(type2, "declname");
  printf("함수명: ");
  json_print(declname);
  printf("\n");

  printf("함수의 리턴 타입: ");
  json_print(names);
  printf("\n");

  printf("함수의 파라미터 없음 \n");
}

// ast.json 파일의 args가 null이 아닌 배열일 때, 파라미터 정보 추출
void args_isNotNull(json_value type1) {
  json_value args = json_get(type1, "args");
  json_value params_obj = json_get(args, "params");
  int params_len = json_len(params_obj);

  // params 배열을 하나씩 순회
  for (int i=0; i < params_len; i++) {
    json_value params = json_get(params_obj, i);
    json_value type1 = json_get(params, "type");
    json_value type2 = json_get(type1, "type");
    int type2_len = json_len(type2);

    // ast.json을 끝까지 파싱할 경우 type->type->type->name 경로와 type->type->name 경로를 구분
    if (type2_len == 3) {
      json_value declname = json_get(type1, "declname");
      printf("함수의 파라미터 변수명: ");
      json_print(declname);
      printf("\n");

      json_value names = json_get(type2, "names");
      printf("함수의 파라미터 타입: ");
      json_print(names);
      printf("\n");
    } else {
      json_value declname = json_get(type2, "declname");
      printf("함수의 파라미터 변수명: ");
      json_print(declname);
      // json_print(declname);
      printf("\n");

      json_value type3 = json_get(type2, "type");
      json_value names = json_get(type3, "names");
      printf("함수의 파라미터 타입: ");
      json_print(names);
      printf("\n");
    }
  }
}

// if문의 개수를 추출하는 함수
int count_if(json_value obj) {
  int count = 0;

  if (json_get_type(obj) == JSON_OBJECT) {
      json_value value = json_get(obj, "_nodetype");
      if (strcmp(json_to_string(value), "If") == 0) {
          count++;
      }

      int obj_len = json_len(obj);
      for (int i = 0; i < obj_len; i++) {
          json_value value = json_get(obj, i);
          count += count_if(value);
      }
  }

  else if (json_get_type(obj) == JSON_ARRAY) {
      int arr_len = json_len(obj);
      for (int i = 0; i < arr_len; i++) {
          json_value value = json_get(obj, i);
          count += count_if(value);
      }
  }

  return count;
}




// 함수 정보 출력하는 함수
void func_info(json_value ext_obj) {
  int ext_len = json_len(ext_obj);

  for (int i = 0; i < ext_len; i++) {
    json_value ext = json_get(ext_obj, i);
    char *_nodetype = json_get_string(ext, "_nodetype");
    if (strcmp(_nodetype, "FuncDef") == 0) {
      printf("%s \n", "=========================");

      json_value body = json_get(ext, "body");
      json_value decl = json_get(ext, "decl");
      json_value type1 = json_get(decl, "type");
      json_value args = json_get(type1, "args");

      if (json_is_null(args)) {
        args_isNull(type1);
      } else {
        notNull_func(type1);
        args_isNotNull(type1);
      }

      int if_count = count_if(body);
      if_condition_count += if_count;
      printf("함수의 if 개수: %d \n", if_count);

      printf("%s \n", "=========================");
      func_count++;
    }
    // 함수 선언 부분
    else if (strcmp(_nodetype, "Decl") == 0)
    {
      json_value type = json_get(ext, "type");

      json_value _nodetype2 = json_get(type, "_nodetype");
      if (strcmp(json_to_string(_nodetype2), "FuncDecl") == 0) {
        printf("%s \n", "=========================");
        json_value args = json_get(type, "args");
        if(json_is_null(args)) {
          args_isNull(type);
        } else {
          notNull_func(type);
          args_isNotNull(type);
        }
        printf("%s \n", "=========================");
        func_count++;
      }
    }
  }
}


int main(void)
{
  json_value json = json_obj();
  json_value ext_obj = json_get(json, "ext");
  func_info(ext_obj);

  printf("함수의 총 개수: %d \n", func_count);
  printf("if문의 총 개수: %d \n", if_condition_count);
  return 0;
}