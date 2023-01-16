/* ==========================================================================
 * Universidade Federal de São Carlos - Campus Sorocaba
 * Disciplina: Organização de Recuperação da Informação
 * Prof. Tiago A. Almeida e Prof. Jurandy Almeida
 *
 * Trabalho 01 - Indexação
 *
 * RA:
 * Aluno:
 * ========================================================================== */

/* Bibliotecas */
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef enum { false, true } bool;

/* Tamanho dos campos dos registros */
/* Campos de tamanho fixo */
#define TAM_DATE 9
#define TAM_DATETIME 13
#define TAM_INT_NUMBER 5
#define TAM_FLOAT_NUMBER 14
#define TAM_ID_CURSO 9
#define TAM_ID_USUARIO 12
#define TAM_TELEFONE 12
#define QTD_MAX_CATEGORIAS 3

/* Campos de tamanho variável (tamanho máximo) */
#define TAM_MAX_NOME 45
#define TAM_MAX_TITULO 52
#define TAM_MAX_INSTITUICAO 52
#define TAM_MAX_MINISTRANTE 51
#define TAM_MAX_EMAIL 45
#define TAM_MAX_CATEGORIA 21

#define MAX_REGISTROS 1000
#define TAM_REGISTRO_USUARIO                                                   \
  (TAM_ID_USUARIO + TAM_MAX_NOME + TAM_MAX_EMAIL + TAM_FLOAT_NUMBER +          \
   TAM_TELEFONE)
#define TAM_REGISTRO_CURSO                                                     \
  (TAM_ID_CURSO + TAM_MAX_TITULO + TAM_MAX_INSTITUICAO + TAM_MAX_MINISTRANTE + \
   TAM_DATE + TAM_INT_NUMBER + TAM_FLOAT_NUMBER +                              \
   QTD_MAX_CATEGORIAS * TAM_MAX_CATEGORIA + 1)
#define TAM_REGISTRO_INSCRICAO                                                 \
  (TAM_ID_CURSO + TAM_ID_USUARIO + TAM_DATETIME + 1 + TAM_DATETIME - 4)
#define TAM_ARQUIVO_USUARIOS (TAM_REGISTRO_USUARIO * MAX_REGISTROS + 1)
#define TAM_ARQUIVO_CURSOS (TAM_REGISTRO_CURSO * MAX_REGISTROS + 1)
#define TAM_ARQUIVO_INSCRICOES (TAM_REGISTRO_INSCRICAO * MAX_REGISTROS + 1)

#define TAM_RRN_REGISTRO 4
#define TAM_CHAVE_USUARIOS_IDX (TAM_ID_USUARIO + TAM_RRN_REGISTRO - 1)
#define TAM_CHAVE_CURSOS_IDX (TAM_ID_CURSO + TAM_RRN_REGISTRO - 1)
#define TAM_CHAVE_INSCRICOES_IDX                                               \
  (TAM_ID_USUARIO + TAM_ID_CURSO + TAM_RRN_REGISTRO - 2)
#define TAM_CHAVE_TITULO_IDX (TAM_MAX_TITULO + TAM_ID_CURSO - 2)
#define TAM_CHAVE_DATA_USUARIO_CURSO_IDX                                       \
  (TAM_DATETIME + TAM_ID_USUARIO + TAM_ID_CURSO - 3)
#define TAM_CHAVE_CATEGORIAS_SECUNDARIO_IDX (TAM_MAX_CATEGORIA - 1)
#define TAM_CHAVE_CATEGORIAS_PRIMARIO_IDX (TAM_ID_CURSO - 1)

#define TAM_ARQUIVO_USUARIOSS_IDX (1000 * MAX_REGISTROS + 1)
#define TAM_ARQUIVO_CURSOSS_IDX (1000 * MAX_REGISTROS + 1)
#define TAM_ARQUIVO_INSCRICOES_IDX (1000 * MAX_REGISTROS + 1)
#define TAM_ARQUIVO_CATEGORIAS_IDX (1000 * MAX_REGISTROS + 1)

/* Mensagens padrões */
#define SUCESSO "OK\n"
#define REGS_PERCORRIDOS "Registros percorridos:"
#define INDICE_CRIADO "Indice %s criado com sucesso!\n"
#define AVISO_NENHUM_REGISTRO_ENCONTRADO "AVISO: Nenhum registro encontrado\n"
#define ERRO_OPCAO_INVALIDA "ERRO: Opcao invalida\n"
#define ERRO_MEMORIA_INSUFICIENTE "ERRO: Memoria insuficiente\n"
#define ERRO_PK_REPETIDA "ERRO: Ja existe um registro com a chave %s\n"
#define ERRO_REGISTRO_NAO_ENCONTRADO "ERRO: Registro nao encontrado\n"
#define ERRO_SALDO_NAO_SUFICIENTE "ERRO: Saldo insuficiente\n"
#define ERRO_CATEGORIA_REPETIDA "ERRO: O curso %s ja possui a categoria %s\n"
#define ERRO_VALOR_INVALIDO "ERRO: Valor invalido\n"
#define ERRO_ARQUIVO_VAZIO "ERRO: Arquivo vazio\n"
#define ERRO_NAO_IMPLEMENTADO "ERRO: Funcao %s nao implementada\n"

/* Registro de Usuario */
typedef struct {
  char id_usuario[TAM_ID_USUARIO];
  char nome[TAM_MAX_NOME];
  char email[TAM_MAX_EMAIL];
  char telefone[TAM_TELEFONE];
  double saldo;
} Usuario;

/* Registro de Curso */
typedef struct {
  char id_curso[TAM_ID_CURSO];
  char titulo[TAM_MAX_TITULO];
  char instituicao[TAM_MAX_INSTITUICAO];
  char ministrante[TAM_MAX_MINISTRANTE];
  char lancamento[TAM_DATE];
  int carga;
  double valor;
  char categorias[QTD_MAX_CATEGORIAS][TAM_MAX_CATEGORIA];
} Curso;

/* Registro de Inscricao */
typedef struct {
  char id_curso[TAM_ID_CURSO];
  char id_usuario[TAM_ID_USUARIO];
  char data_inscricao[TAM_DATETIME];
  char status;
  char data_atualizacao[TAM_DATETIME];
} Inscricao;

/*----- Registros dos índices -----*/

/* Struct para o índice primário dos usuários */
typedef struct {
  char id_usuario[TAM_ID_USUARIO];
  int rrn;
} usuarios_index;

/* Struct para o índice primário dos cursos */
typedef struct {
  char id_curso[TAM_ID_CURSO];
  int rrn;
} cursos_index;

/* Struct para índice primário dos inscricoes */
typedef struct {
  char id_curso[TAM_ID_CURSO];
  char id_usuario[TAM_ID_USUARIO];
  int rrn;
} inscricoes_index;

/* Struct para o índice secundário dos titulos */
typedef struct {
  char titulo[TAM_MAX_TITULO];
  char id_curso[TAM_ID_CURSO];
} titulos_index;

/* Struct para o índice secundário das datas das inscricoes */
typedef struct {
  char data[TAM_DATETIME];
  char id_curso[TAM_ID_CURSO];
  char id_usuario[TAM_ID_USUARIO];
} data_curso_usuario_index;

/* Struct para o índice secundário das categorias (lista invertida) */
typedef struct {
  char chave_secundaria[TAM_MAX_CATEGORIA]; // string com o nome da categoria
  int primeiro_indice;
} categorias_secundario_index;

/* Struct para o índice primário das categorias (lista invertida) */
typedef struct {
  char chave_primaria[TAM_ID_CURSO]; // string com o id do curso
  int proximo_indice;
} categorias_primario_index;

/* Struct para os parâmetros de uma lista invertida */
typedef struct {
  // Ponteiro para o índice secundário
  categorias_secundario_index *categorias_secundario_idx;

  // Ponteiro para o arquivo de índice primário
  categorias_primario_index *categorias_primario_idx;

  // Quantidade de registros de índice secundário
  unsigned qtd_registros_secundario;

  // Quantidade de registros de índice primário
  unsigned qtd_registros_primario;

  // Tamanho de uma chave secundária nesse índice
  unsigned tam_chave_secundaria;

  // Tamanho de uma chave primária nesse índice
  unsigned tam_chave_primaria;

  // Função utilizada para comparar as chaves do índice secundário.
  // Igual às funções de comparação do bsearch e qsort.
  int (*compar)(const void *key, const void *elem);
} inverted_list;

/* Variáveis globais */
/* Arquivos de dados */
char ARQUIVO_USUARIOS[TAM_ARQUIVO_USUARIOS];
char ARQUIVO_CURSOS[TAM_ARQUIVO_CURSOS];
char ARQUIVO_INSCRICOES[TAM_ARQUIVO_INSCRICOES];

/* Índices */
usuarios_index *usuarios_idx = NULL;
cursos_index *cursos_idx = NULL;
inscricoes_index *inscricoes_idx = NULL;
titulos_index *titulo_idx = NULL;
data_curso_usuario_index *data_curso_usuario_idx = NULL;
inverted_list categorias_idx = {
    .categorias_secundario_idx = NULL,
    .categorias_primario_idx = NULL,
    .qtd_registros_secundario = 0,
    .qtd_registros_primario = 0,
    .tam_chave_secundaria = TAM_CHAVE_CATEGORIAS_SECUNDARIO_IDX,
    .tam_chave_primaria = TAM_CHAVE_CATEGORIAS_PRIMARIO_IDX,
};

/* Funções auxiliares para o qsort.
 * Com uma pequena alteração, é possível utilizá-las no bsearch, assim, evitando
 * código duplicado.
 * */
int qsort_usuarios_idx(const void *a, const void *b);
int qsort_cursos_idx(const void *a, const void *b);
int qsort_inscricoes_idx(const void *a, const void *b);
int qsort_titulo_idx(const void *a, const void *b);
int qsort_data_curso_usuario_idx(const void *a, const void *b);
int qsort_data_idx(const void *a, const void *b);
int qsort_categorias_secundario_idx(const void *a, const void *b);

/* Contadores */
unsigned qtd_registros_usuarios = 0;
unsigned qtd_registros_cursos = 0;
unsigned qtd_registros_inscricoes = 0;

/* Funções de geração determinística de números pseudo-aleatórios */
uint64_t prng_seed;

void prng_srand(uint64_t value) { prng_seed = value; }

uint64_t prng_rand() {
  // https://en.wikipedia.org/wiki/Xorshift#xorshift*
  uint64_t x =
      prng_seed; // O estado deve ser iniciado com um valor diferente de 0
  x ^= x >> 12;  // a
  x ^= x << 25;  // b
  x ^= x >> 27;  // c
  prng_seed = x;
  return x * UINT64_C(0x2545F4914F6CDD1D);
}

/**
 * Gera um <a
 * href="https://en.wikipedia.org/wiki/Universally_unique_identifier">UUID
 * Version-4 Variant-1</a>
 * (<i>string</i> aleatória) de 36 caracteres utilizando o gerador de números
 * pseudo-aleatórios <a
 * href="https://en.wikipedia.org/wiki/Xorshift#xorshift*">xorshift*</a>. O UUID
 * é escrito na <i>string</i> fornecida como parâmetro.<br /> <br /> Exemplo de
 * uso:<br /> <code> char chave_aleatoria[37];<br />
 * new_uuid(chave_aleatoria);<br />
 * printf("chave aleatória: %s&#92;n", chave_aleatoria);<br />
 * </code>
 *
 * @param buffer String de tamanho 37 no qual será escrito
 * o UUID. É terminado pelo caractere <code>\0</code>.
 */
void new_uuid(char buffer[37]) {
  uint64_t r1 = prng_rand();
  uint64_t r2 = prng_rand();

  sprintf(buffer, "%08x-%04x-%04lx-%04lx-%012lx", (uint32_t)(r1 >> 32),
          (uint16_t)(r1 >> 16), 0x4000 | (r1 & 0x0fff), 0x8000 | (r2 & 0x3fff),
          r2 >> 16);
}

/* Funções de manipulação de data */
time_t epoch;

#define YEAR0 1900
#define EPOCH_YR 1970
#define SECS_DAY (24L * 60L * 60L)
#define LEAPYEAR(year) (!((year) % 4) && (((year) % 100) || !((year) % 400)))
#define YEARSIZE(year) (LEAPYEAR(year) ? 366 : 365)

#define TIME_MAX 2147483647L

long _dstbias = 0;  // Offset for Daylight Saving Time
long _timezone = 0; // Difference in seconds between GMT and local time

const int _ytab[2][12] = {{31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
                          {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}};

struct tm *gmtime_r(const time_t *timer, struct tm *tmbuf) {
  // based on http://www.jbox.dk/sanos/source/lib/time.c.html
  time_t time = *timer;
  unsigned long dayclock, dayno;
  int year = EPOCH_YR;

  dayclock = (unsigned long)time % SECS_DAY;
  dayno = (unsigned long)time / SECS_DAY;

  tmbuf->tm_sec = dayclock % 60;
  tmbuf->tm_min = (dayclock % 3600) / 60;
  tmbuf->tm_hour = dayclock / 3600;
  tmbuf->tm_wday = (dayno + 4) % 7; // Day 0 was a thursday
  while (dayno >= (unsigned long)YEARSIZE(year)) {
    dayno -= YEARSIZE(year);
    year++;
  }
  tmbuf->tm_year = year - YEAR0;
  tmbuf->tm_yday = dayno;
  tmbuf->tm_mon = 0;
  while (dayno >= (unsigned long)_ytab[LEAPYEAR(year)][tmbuf->tm_mon]) {
    dayno -= _ytab[LEAPYEAR(year)][tmbuf->tm_mon];
    tmbuf->tm_mon++;
  }
  tmbuf->tm_mday = dayno + 1;
  tmbuf->tm_isdst = 0;
  tmbuf->tm_gmtoff = 0;
  tmbuf->tm_zone = "UTC";
  return tmbuf;
}

time_t mktime(struct tm *tmbuf) {
  // based on http://www.jbox.dk/sanos/source/lib/time.c.html
  long day, year;
  int tm_year;
  int yday, month;
  /*unsigned*/ long seconds;
  int overflow;
  long dst;

  tmbuf->tm_min += tmbuf->tm_sec / 60;
  tmbuf->tm_sec %= 60;
  if (tmbuf->tm_sec < 0) {
    tmbuf->tm_sec += 60;
    tmbuf->tm_min--;
  }
  tmbuf->tm_hour += tmbuf->tm_min / 60;
  tmbuf->tm_min = tmbuf->tm_min % 60;
  if (tmbuf->tm_min < 0) {
    tmbuf->tm_min += 60;
    tmbuf->tm_hour--;
  }
  day = tmbuf->tm_hour / 24;
  tmbuf->tm_hour = tmbuf->tm_hour % 24;
  if (tmbuf->tm_hour < 0) {
    tmbuf->tm_hour += 24;
    day--;
  }
  tmbuf->tm_year += tmbuf->tm_mon / 12;
  tmbuf->tm_mon %= 12;
  if (tmbuf->tm_mon < 0) {
    tmbuf->tm_mon += 12;
    tmbuf->tm_year--;
  }
  day += (tmbuf->tm_mday - 1);
  while (day < 0) {
    if (--tmbuf->tm_mon < 0) {
      tmbuf->tm_year--;
      tmbuf->tm_mon = 11;
    }
    day += _ytab[LEAPYEAR(YEAR0 + tmbuf->tm_year)][tmbuf->tm_mon];
  }
  while (day >= _ytab[LEAPYEAR(YEAR0 + tmbuf->tm_year)][tmbuf->tm_mon]) {
    day -= _ytab[LEAPYEAR(YEAR0 + tmbuf->tm_year)][tmbuf->tm_mon];
    if (++(tmbuf->tm_mon) == 12) {
      tmbuf->tm_mon = 0;
      tmbuf->tm_year++;
    }
  }
  tmbuf->tm_mday = day + 1;
  year = EPOCH_YR;
  if (tmbuf->tm_year < year - YEAR0)
    return (time_t)-1;
  seconds = 0;
  day = 0; // Means days since day 0 now
  overflow = 0;

  // Assume that when day becomes negative, there will certainly
  // be overflow on seconds.
  // The check for overflow needs not to be done for leapyears
  // divisible by 400.
  // The code only works when year (1970) is not a leapyear.
  tm_year = tmbuf->tm_year + YEAR0;

  if (TIME_MAX / 365 < tm_year - year)
    overflow++;
  day = (tm_year - year) * 365;
  if (TIME_MAX - day < (tm_year - year) / 4 + 1)
    overflow++;
  day += (tm_year - year) / 4 + ((tm_year % 4) && tm_year % 4 < year % 4);
  day -=
      (tm_year - year) / 100 + ((tm_year % 100) && tm_year % 100 < year % 100);
  day +=
      (tm_year - year) / 400 + ((tm_year % 400) && tm_year % 400 < year % 400);

  yday = month = 0;
  while (month < tmbuf->tm_mon) {
    yday += _ytab[LEAPYEAR(tm_year)][month];
    month++;
  }
  yday += (tmbuf->tm_mday - 1);
  if (day + yday < 0)
    overflow++;
  day += yday;

  tmbuf->tm_yday = yday;
  tmbuf->tm_wday = (day + 4) % 7; // Day 0 was thursday (4)

  seconds = ((tmbuf->tm_hour * 60L) + tmbuf->tm_min) * 60L + tmbuf->tm_sec;

  if ((TIME_MAX - seconds) / SECS_DAY < day)
    overflow++;
  seconds += day * SECS_DAY;

  // Now adjust according to timezone and daylight saving time
  if (((_timezone > 0) && (TIME_MAX - _timezone < seconds)) ||
      ((_timezone < 0) && (seconds < -_timezone))) {
    overflow++;
  }
  seconds += _timezone;

  if (tmbuf->tm_isdst) {
    dst = _dstbias;
  } else {
    dst = 0;
  }

  if (dst > seconds)
    overflow++; // dst is always non-negative
  seconds -= dst;

  if (overflow)
    return (time_t)-1;

  if ((time_t)seconds != seconds)
    return (time_t)-1;
  return (time_t)seconds;
}

bool set_time(char *date) {
  // http://www.cplusplus.com/reference/ctime/mktime/
  struct tm tm_;

  if (strlen(date) == TAM_DATETIME - 1 &&
      sscanf(date, "%4d%2d%2d%2d%2d", &tm_.tm_year, &tm_.tm_mon, &tm_.tm_mday,
             &tm_.tm_hour, &tm_.tm_min) == 5) {
    tm_.tm_year -= 1900;
    tm_.tm_mon -= 1;
    tm_.tm_sec = 0;
    tm_.tm_isdst = -1;
    epoch = mktime(&tm_);
    return true;
  }
  return false;
}

void tick_time() {
  epoch += prng_rand() % 864000; // 10 dias
}

/**
 * Escreve a <i>data</i> atual no formato <code>AAAAMMDD</code> em uma
 * <i>string</i> fornecida como parâmetro.<br /> <br /> Exemplo de uso:<br />
 * <code>
 * char timestamp[TAM_DATE];<br />
 * current_date(timestamp);<br />
 * printf("data atual: %s&#92;n", timestamp);<br />
 * </code>
 *
 * @param buffer String de tamanho <code>TAM_DATE</code> no qual será escrita
 * a <i>timestamp</i>. É terminado pelo caractere <code>\0</code>.
 */
void current_date(char buffer[TAM_DATE]) {
  // http://www.cplusplus.com/reference/ctime/strftime/
  // http://www.cplusplus.com/reference/ctime/gmtime/
  // AAAA MM DD
  // %Y   %m %d
  struct tm tm_;
  if (gmtime_r(&epoch, &tm_) != NULL)
    strftime(buffer, TAM_DATE, "%Y%m%d", &tm_);
}

/**
 * Escreve a <i>data</i> e a <i>hora</i> atual no formato
 * <code>AAAAMMDDHHMM</code> em uma <i>string</i> fornecida como parâmetro.<br
 * /> <br /> Exemplo de uso:<br /> <code> char timestamp[TAM_DATETIME];<br />
 * current_datetime(timestamp);<br />
 * printf("data e hora atual: %s&#92;n", timestamp);<br />
 * </code>
 *
 * @param buffer String de tamanho <code>TAM_DATETIME</code> no qual será
 * escrita a <i>timestamp</i>. É terminado pelo caractere <code>\0</code>.
 */
void current_datetime(char buffer[TAM_DATETIME]) {
  // http://www.cplusplus.com/reference/ctime/strftime/
  // http://www.cplusplus.com/reference/ctime/gmtime/
  // AAAA MM DD HH MM
  // %Y   %m %d %H %M
  struct tm tm_;
  if (gmtime_r(&epoch, &tm_) != NULL)
    strftime(buffer, TAM_DATETIME, "%Y%m%d%H%M", &tm_);
}

/* Remove comentários (--) e caracteres whitespace do começo e fim de uma string
 */
void clear_input(char *str) {
  char *ptr = str;
  int len = 0;

  for (; ptr[len]; ++len) {
    if (strncmp(&ptr[len], "--", 2) == 0) {
      ptr[len] = '\0';
      break;
    }
  }

  while (len - 1 > 0 && isspace(ptr[len - 1]))
    ptr[--len] = '\0';

  while (*ptr && isspace(*ptr))
    ++ptr, --len;

  memmove(str, ptr, len + 1);
}

/* ==========================================================================
 * ========================= PROTÓTIPOS DAS FUNÇÕES =========================
 * ========================================================================== */

/* Cria o índice respectivo */
void criar_usuarios_idx();           //  ok
void criar_cursos_idx();             // ok
void criar_inscricoes_idx();         // ok
void criar_titulo_idx();             // ok
void criar_data_curso_usuario_idx(); // ok
void criar_categorias_idx();         // ok

/* Exibe um registro com base no RRN */
bool exibir_usuario(int rrn);
bool exibir_curso(int rrn);
bool exibir_inscricao(int rrn);

/* Recupera do arquivo o registro com o RRN informado
 * e retorna os dados nas structs Usuario, Curso e Inscricao */
Usuario recuperar_registro_usuario(int rrn);     // ok
Curso recuperar_registro_curso(int rrn);         // ok
Inscricao recuperar_registro_inscricao(int rrn); // ok

/* Escreve em seu respectivo arquivo na posição informada (RRN) */
void escrever_registro_usuario(Usuario u, int rrn);     // ok
void escrever_registro_curso(Curso j, int rrn);         // ok
void escrever_registro_inscricao(Inscricao c, int rrn); // ok

/* Funções principais */
void cadastrar_usuario_menu(char *id_usuario, char *nome, char *email,
                            char *telefone);
void cadastrar_telefone_menu(char *id_usuario, char *telefone);
void remover_usuario_menu(char *id_usuario);
void cadastrar_curso_menu(char *titulo, char *instituicao, char *ministrante,
                          char *lancamento, int carga, double valor);
void adicionar_saldo_menu(char *id_usuario, double valor);
void inscrever_menu(char *id_curso, char *id_usuario);
void cadastrar_categoria_menu(char *titulo, char *categoria);
void atualizar_status_inscricoes_menu(char *id_usuario, char *titulo,
                                      char status);

/* Busca */
void buscar_usuario_id_menu(char *id_usuario);
void buscar_curso_id_menu(char *id_curso);
void buscar_curso_titulo_menu(char *titulo);

/* Listagem */
void listar_usuarios_id_menu();
void listar_cursos_categorias_menu(char *categoria);
void listar_inscricoes_periodo_menu(char *data_inicio, char *data_fim);

/* Liberar espaço */
void liberar_espaco_menu();

/* Imprimir arquivos de dados */
void imprimir_arquivo_usuarios_menu();
void imprimir_arquivo_cursos_menu();
void imprimir_arquivo_inscricoes_menu();

/* Imprimir índices primários */
void imprimir_usuarios_idx_menu();
void imprimir_cursos_idx_menu();
void imprimir_inscricoes_idx_menu();

/* Imprimir índices secundários */
void imprimir_titulo_idx_menu();
void imprimir_data_curso_usuario_idx_menu();
void imprimir_categorias_secundario_idx_menu();
void imprimir_categorias_primario_idx_menu();

/* Liberar memória e encerrar programa */
void liberar_memoria_menu();

/* Funções de manipulação de Lista Invertida */
/**
 * Responsável por inserir duas chaves (chave_secundaria e chave_primaria) em
 * uma Lista Invertida (t).<br /> Atualiza os parâmetros dos índices primário e
 * secundário conforme necessário.<br /> As chaves a serem inseridas devem estar
 * no formato correto e com tamanho t->tam_chave_primario e
 * t->tam_chave_secundario.<br /> O funcionamento deve ser genérico para
 * qualquer Lista Invertida, adaptando-se para os diferentes parâmetros
 * presentes em seus structs.<br />
 *
 * @param chave_secundaria Chave a ser buscada (caso exista) ou inserida (caso
 * não exista) no registro secundário da Lista Invertida.
 * @param chave_primaria Chave a ser inserida no registro primário da Lista
 * Invertida.
 * @param t Ponteiro para a Lista Invertida na qual serão inseridas as chaves.
 */
void inverted_list_insert(char *chave_secundaria, char *chave_primaria,
                          inverted_list *t);

/**
 * Responsável por buscar uma chave no índice secundário de uma Lista invertida
 * (T). O valor de retorno indica se a chave foi encontrada ou não. O ponteiro
 * para o int result pode ser fornecido opcionalmente, e conterá o índice
 * inicial das chaves no registro primário.<br /> <br /> Exemplos de uso:<br />
 * <code>
 * // Exemplo 1. A chave encontrada deverá ser retornada e o caminho não deve
 * ser informado.<br />
 * ...<br />
 * int result;<br />
 * bool found = inverted_list_secondary_search(&result, false, categoria,
 * &categorias_idx);<br />
 * ...<br />
 * <br />
 * // Exemplo 2. Não há interesse na chave encontrada, apenas se ela existe, e o
 * caminho não deve ser informado.<br />
 * ...<br />
 * bool found = inverted_list_secondary_search(NULL, false, categoria,
 * &categorias_idx);<br />
 * ...<br />
 * <br />
 * // Exemplo 3. Há interesse no caminho feito para encontrar a chave.<br />
 * ...<br />
 * int result;<br />
 * bool found = inverted_list_secondary_search(&result, true, categoria,
 * &categorias_idx);<br />
 * </code>
 *
 * @param result Ponteiro para ser escrito o índice inicial (primeira
 * ocorrência) das chaves do registro primário. É ignorado caso NULL.
 * @param exibir_caminho Indica se o caminho percorrido deve ser impresso.
 * @param chave_secundaria Chave a ser buscada.
 * @param t Ponteiro para o índice do tipo Lista invertida no qual será buscada
 * a chave.
 * @return Indica se a chave foi encontrada.
 */
bool inverted_list_secondary_search(int *result, bool exibir_caminho,
                                    char *chave_secundaria, inverted_list *t);

/**
 * Responsável por percorrer o índice primário de uma Lista invertida (T). O
 * valor de retorno indica a quantidade de chaves encontradas. O ponteiro para o
 * vetor de strings result pode ser fornecido opcionalmente, e será populado com
 * a lista de todas as chaves encontradas. O ponteiro para o inteiro
 * indice_final também pode ser fornecido opcionalmente, e deve conter o índice
 * do último campo da lista encadeada da chave primaria fornecida (isso é útil
 * na inserção de um novo registro).<br /> <br /> Exemplos de uso:<br /> <code>
 * // Exemplo 1. As chaves encontradas deverão ser retornadas e tanto o caminho
 * quanto o indice_final não devem ser informados.<br />
 * ...<br />
 * char chaves[TAM_CHAVE_CATEGORIAS_PRIMARIO_IDX][MAX_REGISTROS];<br />
 * int qtd = inverted_list_primary_search(chaves, false, indice, NULL,
 * &categorias_idx);<br />
 * ...<br />
 * <br />
 * // Exemplo 2. Não há interesse nas chaves encontradas, apenas no
 * indice_final, e o caminho não deve ser informado.<br />
 * ...<br />
 * int indice_final;
 * int qtd = inverted_list_primary_search(NULL, false, indice, &indice_final,
 * &categorias_idx);<br />
 * ...<br />
 * <br />
 * // Exemplo 3. Há interesse nas chaves encontradas e no caminho feito.<br />
 * ...<br />
 * char chaves[TAM_CHAVE_CATEGORIAS_PRIMARIO_IDX][MAX_REGISTROS];<br />
 * int qtd = inverted_list_primary_search(chaves, true, indice, NULL,
 * &categorias_idx);<br />
 * ...<br />
 * <br />
 * </code>
 *
 * @param result Ponteiro para serem escritas as chaves encontradas. É ignorado
 * caso NULL.
 * @param exibir_caminho Indica se o caminho percorrido deve ser impresso.
 * @param indice Índice do primeiro registro da lista encadeada a ser procurado.
 * @param indice_final Ponteiro para ser escrito o índice do último registro
 * encontrado (cujo campo indice é -1). É ignorado caso NULL.
 * @param t Ponteiro para o índice do tipo Lista invertida no qual será buscada
 * a chave.
 * @return Indica a quantidade de chaves encontradas.
 */
int inverted_list_primary_search(
    char result[][TAM_CHAVE_CATEGORIAS_PRIMARIO_IDX], bool exibir_caminho,
    int indice, int *indice_final, inverted_list *t);

/**
 * Preenche uma string str com o caractere pad para completar o tamanho size.<br
 * />
 *
 * @param str Ponteiro para a string a ser manipulada.
 * @param pad Caractere utilizado para fazer o preenchimento à direita.
 * @param size Tamanho desejado para a string.
 */
char *strpadright(char *str, char pad, unsigned size);

/**
 * Converte uma string str para letras maiúsculas.<br />
 *
 * @param str Ponteiro para a string a ser manipulada.
 */
char *strupr(char *str);

/**
 * Converte uma string str para letras minúsculas.<br />
 *
 * @param str Ponteiro para a string a ser manipulada.
 */
char *strlower(char *str);

/* Funções de busca binária */
/**
 * Função Genérica de busca binária, que aceita parâmetros genéricos (assinatura
 * baseada na função bsearch da biblioteca C).
 *
 * @param key Chave de busca genérica.
 * @param base0 Base onde ocorrerá a busca, por exemplo, um ponteiro para um
 * vetor.
 * @param nmemb Número de elementos na base.
 * @param size Tamanho do tipo do elemento na base, dica: utilize a função
 * sizeof().
 * @param compar Ponteiro para a função que será utilizada nas comparações.
 * @param exibir_caminho Indica se o caminho percorrido deve ser impresso. Se
 * true, imprime as posições avaliadas durante a busca, até que todas sejam
 * visitadas ou o elemento seja encontrado (pela 1a vez). Caso o número de
 * elementos seja par (p.ex, 10 elementos), então há 2 (duas) possibilidades
 * para a posição da mediana dos elementos (p.ex., 5a ou 6a posição se o total
 * fosse 10). Neste caso, SEMPRE escolha a posição mais à direita (p.ex., a
 * posição 6 caso o total for 10).
 * @param retorno_se_nao_encontrado Caso o elemento NÃO seja encontrado, indica
 * qual valor deve ser retornado. As opções são: -1 = predecessor : retorna o
 * elemento PREDECESSOR (o que apareceria imediatamente antes do elemento
 * procurado, caso fosse encontrado). 0 = nulo : retorna NULL. [modo padrão] +1
 * = sucessor : retorna o elemento SUCESSOR (o que apareceria imediatamente
 * depois do elemento procurado, caso fosse encontrado).
 * @return Retorna o elemento encontrado ou NULL se não encontrou.
 */
void *busca_binaria(const void *key, const void *base0, size_t nmemb,
                    size_t size, int (*compar)(const void *, const void *),
                    bool exibir_caminho, int retorno_se_nao_encontrado);

/* <<< COLOQUE AQUI OS DEMAIS PROTÓTIPOS DE FUNÇÕES, SE NECESSÁRIO >>> */

/* ==========================================================================
 * ============================ FUNÇÃO PRINCIPAL ============================
 * =============================== NÃO ALTERAR ============================== */

int main() {
  // variáveis utilizadas pelo interpretador de comandos
  char input[500];
  uint64_t seed = 2;
  char datetime[TAM_DATETIME] = "202103181430"; // UTC 18/03/2021 14:30:00
  char id_usuario[TAM_ID_USUARIO];
  char nome[TAM_MAX_NOME];
  char email[TAM_MAX_EMAIL];
  char telefone[TAM_TELEFONE];
  char id_curso[TAM_ID_CURSO];
  char titulo[TAM_MAX_TITULO];
  char instituicao[TAM_MAX_INSTITUICAO];
  char ministrante[TAM_MAX_MINISTRANTE];
  char lancamento[TAM_DATE];
  char categoria[TAM_MAX_CATEGORIA];
  int carga;
  double valor;
  char data_inicio[TAM_DATETIME];
  char data_fim[TAM_DATETIME];
  char status;

  scanf("SET ARQUIVO_USUARIOS TO '%[^']';\n", ARQUIVO_USUARIOS);
  int temp_len = strlen(ARQUIVO_USUARIOS);
  qtd_registros_usuarios = temp_len / TAM_REGISTRO_USUARIO;
  ARQUIVO_USUARIOS[temp_len] = '\0';

  scanf("SET ARQUIVO_CURSOS TO '%[^']';\n", ARQUIVO_CURSOS);
  temp_len = strlen(ARQUIVO_CURSOS);
  qtd_registros_cursos = temp_len / TAM_REGISTRO_CURSO;
  ARQUIVO_CURSOS[temp_len] = '\0';

  scanf("SET ARQUIVO_INSCRICOES TO '%[^']';\n", ARQUIVO_INSCRICOES);
  temp_len = strlen(ARQUIVO_INSCRICOES);
  qtd_registros_inscricoes = temp_len / TAM_REGISTRO_INSCRICAO;
  ARQUIVO_INSCRICOES[temp_len] = '\0';

  // inicialização do gerador de números aleatórios e função de datas
  prng_srand(seed);
  putenv("TZ=UTC");
  set_time(datetime);

  criar_usuarios_idx();
  criar_cursos_idx();
  criar_inscricoes_idx();
  criar_titulo_idx();
  criar_data_curso_usuario_idx();
  criar_categorias_idx();

  while (1) {
    fgets(input, 500, stdin);
    printf("%s", input);
    clear_input(input);

    if (strcmp("", input) == 0)
      continue; // não avança o tempo nem imprime o comando este seja em branco

    /* Funções principais */
    if (sscanf(
            input,
            "INSERT INTO usuarios VALUES ('%[^']', '%[^']', '%[^']', '%[^']');",
            id_usuario, nome, email, telefone) == 4)
      cadastrar_usuario_menu(id_usuario, nome, email, telefone);
    else if (sscanf(input,
                    "INSERT INTO usuarios VALUES ('%[^']', '%[^']', '%[^']');",
                    id_usuario, nome, email) == 3) {
      strcpy(telefone, "");
      strpadright(telefone, '*', TAM_TELEFONE - 1);
      cadastrar_usuario_menu(id_usuario, nome, email, telefone);
    } else if (sscanf(input,
                      "UPDATE usuarios SET telefone = '%[^']' WHERE id_usuario "
                      "= '%[^']';",
                      telefone, id_usuario) == 2)
      cadastrar_telefone_menu(id_usuario, telefone);
    else if (sscanf(input, "DELETE FROM usuarios WHERE id_usuario = '%[^']';",
                    id_usuario) == 1)
      remover_usuario_menu(id_usuario);
    else if (sscanf(input,
                    "INSERT INTO cursos VALUES ('%[^']', '%[^']', '%[^']', "
                    "'%[^']', %d, %lf);",
                    titulo, instituicao, ministrante, lancamento, &carga,
                    &valor) == 6)
      cadastrar_curso_menu(titulo, instituicao, ministrante, lancamento, carga,
                           valor);
    else if (sscanf(input,
                    "UPDATE usuarios SET saldo = saldo + %lf WHERE id_usuario "
                    "= '%[^']';",
                    &valor, id_usuario) == 2)
      adicionar_saldo_menu(id_usuario, valor);
    else if (sscanf(input, "INSERT INTO inscricoes VALUES ('%[^']', '%[^']');",
                    id_curso, id_usuario) == 2)
      inscrever_menu(id_curso, id_usuario);
    else if (sscanf(input,
                    "UPDATE cursos SET categorias = array_append(categorias, "
                    "'%[^']') WHERE titulo = '%[^']';",
                    categoria, titulo) == 2)
      cadastrar_categoria_menu(titulo, categoria);
    else if (sscanf(input,
                    "UPDATE inscricoes SET status = '%c' WHERE id_curso = "
                    "(SELECT id_curso FROM cursos WHERE titulo = '%[^']') AND "
                    "id_usuario = '%[^']';",
                    &status, titulo, id_usuario) == 3)
      atualizar_status_inscricoes_menu(id_usuario, titulo, status);

    /* Busca */
    else if (sscanf(input, "SELECT * FROM usuarios WHERE id_usuario = '%[^']';",
                    id_usuario) == 1)
      buscar_usuario_id_menu(id_usuario);
    else if (sscanf(input, "SELECT * FROM cursos WHERE id_curso = '%[^']';",
                    id_curso) == 1)
      buscar_curso_id_menu(id_curso);
    else if (sscanf(input, "SELECT * FROM cursos WHERE titulo = '%[^']';",
                    titulo) == 1)
      buscar_curso_titulo_menu(titulo);

    /* Listagem */
    else if (strcmp("SELECT * FROM usuarios ORDER BY id_usuario ASC;", input) ==
             0)
      listar_usuarios_id_menu();
    else if (sscanf(input,
                    "SELECT * FROM cursos WHERE '%[^']' = ANY (categorias) "
                    "ORDER BY id_curso ASC;",
                    categoria) == 1)
      listar_cursos_categorias_menu(categoria);
    else if (sscanf(input,
                    "SELECT * FROM inscricoes WHERE data_inscricao BETWEEN "
                    "'%[^']' AND '%[^']' ORDER BY data_inscricao ASC;",
                    data_inicio, data_fim) == 2)
      listar_inscricoes_periodo_menu(data_inicio, data_fim);

    /* Liberar espaço */
    else if (strcmp("VACUUM usuarios;", input) == 0)
      liberar_espaco_menu();

    /* Imprimir arquivos de dados */
    else if (strcmp("\\echo file ARQUIVO_USUARIOS", input) == 0)
      imprimir_arquivo_usuarios_menu();
    else if (strcmp("\\echo file ARQUIVO_CURSOS", input) == 0)
      imprimir_arquivo_cursos_menu();
    else if (strcmp("\\echo file ARQUIVO_INSCRICOES", input) == 0)
      imprimir_arquivo_inscricoes_menu();

    /* Imprimir índices primários */
    else if (strcmp("\\echo index usuarios_idx", input) == 0)
      imprimir_usuarios_idx_menu();
    else if (strcmp("\\echo index cursos_idx", input) == 0)
      imprimir_cursos_idx_menu();
    else if (strcmp("\\echo index inscricoes_idx", input) == 0)
      imprimir_inscricoes_idx_menu();

    /* Imprimir índices secundários */
    else if (strcmp("\\echo index titulo_idx", input) == 0)
      imprimir_titulo_idx_menu();
    else if (strcmp("\\echo index data_curso_usuario_idx", input) == 0)
      imprimir_data_curso_usuario_idx_menu();
    else if (strcmp("\\echo index categorias_secundario_idx", input) == 0)
      imprimir_categorias_secundario_idx_menu();
    else if (strcmp("\\echo index categorias_primario_idx", input) == 0)
      imprimir_categorias_primario_idx_menu();

    /* Liberar memória eventualmente alocada e encerrar programa */
    else if (strcmp("\\q", input) == 0) {
      liberar_memoria_menu();
      return 0;
    } else if (sscanf(input, "SET SRAND %lu;", &seed) == 1) {
      prng_srand(seed);
      printf(SUCESSO);
      continue;
    } else if (sscanf(input, "SET TIME '%[^']';", datetime) == 1) {
      if (set_time(datetime))
        printf(SUCESSO);
      else
        printf(ERRO_VALOR_INVALIDO);
      continue;
    } else
      printf(ERRO_OPCAO_INVALIDA);

    tick_time();
  }
}

/* ========================================================================== */

/* Cria o índice primário usuarios_idx */
void criar_usuarios_idx() {
  if (!usuarios_idx)
    usuarios_idx = malloc(MAX_REGISTROS * sizeof(usuarios_index));

  if (!usuarios_idx) {
    printf(ERRO_MEMORIA_INSUFICIENTE);
    exit(1);
  }

  for (unsigned i = 0; i < qtd_registros_usuarios; ++i) {
    Usuario u = recuperar_registro_usuario(i);

    if (strncmp(u.id_usuario, "*|", 2) == 0)
      usuarios_idx[i].rrn = -1; // registro excluído
    else
      usuarios_idx[i].rrn = i;

    strcpy(usuarios_idx[i].id_usuario, u.id_usuario);
  }

  qsort(usuarios_idx, qtd_registros_usuarios, sizeof(usuarios_index),
        qsort_usuarios_idx);
  printf(INDICE_CRIADO, "usuarios_idx");
}

/* Cria o índice primário cursos_idx */
void criar_cursos_idx() {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  if (!cursos_idx)
    cursos_idx = malloc(MAX_REGISTROS * sizeof(cursos_idx));
  if (!cursos_idx) {
    printf(ERRO_MEMORIA_INSUFICIENTE);
    exit(1);
  }
  for (unsigned i = 0; i < qtd_registros_cursos; i++) {
    Curso c = recuperar_registro_curso(i);

    if (strncmp(c.id_curso, "*|", 2) == 0)
      cursos_idx[i].rrn = -1;
    else
      cursos_idx[i].rrn = i;

    strcpy(cursos_idx[i].id_curso, c.id_curso);
  }
  qsort(cursos_idx, qtd_registros_cursos, sizeof(cursos_idx), qsort_cursos_idx);
  printf(INDICE_CRIADO, "cursos_idx");
}

/* Cria o índice primário inscricoes_idx */
void criar_inscricoes_idx() {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  if (!inscricoes_idx)
    inscricoes_idx = malloc(sizeof(inscricoes_idx) * MAX_REGISTROS);
  if (!inscricoes_idx) {
    printf(ERRO_MEMORIA_INSUFICIENTE);
    exit(1);
  }

  for (unsigned i = 0; i < qtd_registros_inscricoes; i++) {
    Inscricao ins = recuperar_registro_inscricao(i);

    if (strncmp(ins.id_curso, "*|", 2)) {
      strcpy(inscricoes_idx[i].id_curso, ins.id_curso);
      strcpy(inscricoes_idx[i].id_usuario, ins.id_usuario);
    }
  }
  qsort(inscricoes_idx, qtd_registros_inscricoes, sizeof(inscricoes_idx),
        qsort_inscricoes_idx);
  printf(INDICE_CRIADO, "inscricoes_idx");
}

/* Cria o índice secundário titulo_idx */
void criar_titulo_idx() {
  if (!titulo_idx)
    titulo_idx = malloc(sizeof(titulos_index) * MAX_REGISTROS);
  if (!titulo_idx) {
    printf(ERRO_MEMORIA_INSUFICIENTE);
    exit(1);
  }

  for (unsigned i = 0; i < qtd_registros_cursos; i++) {
    if (cursos_idx[i].rrn != -1) {
      Curso c = recuperar_registro_curso(i);
      strcpy(titulo_idx[i].id_curso, c.id_curso);
      strcpy(titulo_idx[i].titulo, c.titulo);
    }
  }

  qsort(titulo_idx, qtd_registros_cursos, sizeof(titulos_index),
        qsort_titulo_idx);

  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  printf(INDICE_CRIADO, "titulo_idx");
}

/* Cria o índice secundário data_curso_usuario_idx */
void criar_data_curso_usuario_idx() {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  if (!data_curso_usuario_idx)
    data_curso_usuario_idx =
        malloc(sizeof(data_curso_usuario_index) * MAX_REGISTROS);
  if (!data_curso_usuario_idx) {
    printf(ERRO_MEMORIA_INSUFICIENTE);
    exit(1);
  }
  for (unsigned i = 0; i < qtd_registros_inscricoes; i++) {
    if (!(inscricoes_idx[i].rrn == -1)) {
      Inscricao ins = recuperar_registro_inscricao(i);
      strcpy(data_curso_usuario_idx[i].id_curso, ins.id_curso);
      strcpy(data_curso_usuario_idx[i].id_usuario, ins.id_usuario);
      strcpy(data_curso_usuario_idx[i].data, ins.data_inscricao);
    }
  }
  qsort(data_curso_usuario_idx, qtd_registros_inscricoes,
        sizeof(data_curso_usuario_idx), qsort_data_curso_usuario_idx);
  printf(INDICE_CRIADO, "data_usuario_curso_idx");
}

/* Cria os índices (secundário e primário) de categorias_idx */
void criar_categorias_idx() {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  categorias_idx.qtd_registros_primario = 0;
  categorias_idx.qtd_registros_secundario = 0; // categorias

  categorias_idx.tam_chave_primaria = TAM_ID_CURSO;
  categorias_idx.tam_chave_secundaria = TAM_MAX_CATEGORIA;

  if (!categorias_idx.categorias_primario_idx)
    categorias_idx.categorias_primario_idx =
        malloc(sizeof(categorias_primario_index) * MAX_REGISTROS);
  if (!categorias_idx.categorias_primario_idx) {
    printf(ERRO_MEMORIA_INSUFICIENTE);
    exit(1);
  }

  if (!categorias_idx.categorias_secundario_idx)
    categorias_idx.categorias_secundario_idx =
        malloc(sizeof(categorias_secundario_index) * MAX_REGISTROS);
  if (!categorias_idx.categorias_secundario_idx) {
    printf(ERRO_MEMORIA_INSUFICIENTE);
    exit(1);
  }
  for (unsigned i = 0; i < qtd_registros_cursos; i++) {
    Curso c = recuperar_registro_curso(i);
    // strcpy(categorias_idx.categorias_primario_idx[i].chave_primaria,
    // c.id_curso);

    // categorias
    for (int k = 0; k < QTD_MAX_CATEGORIAS; k++) {

      char *p;
      strcpy(p, c.categorias[i]);
      // checar se a categoria já existe e pegar a posição caso exista
      if (strcmp(p, "") == 0)
        break;
      bool existe = false;
      int pos;
      for (int j = 0; i < categorias_idx.qtd_registros_secundario; j++) {
        if (strcmp(categorias_idx.categorias_secundario_idx[j].chave_secundaria,
                   p) == 0) {
          existe = true;
          pos = j;
          break;
        }
      }
      if (!existe) { // se não existe colocar na ultima posição como categoria
                     // nova

        strcpy(categorias_idx
                   .categorias_secundario_idx[categorias_idx
                                                  .qtd_registros_secundario]
                   .chave_secundaria,
               p);
        categorias_idx
            .categorias_secundario_idx[categorias_idx.qtd_registros_secundario]
            .primeiro_indice = categorias_idx.qtd_registros_primario;
        categorias_idx.qtd_registros_secundario++;

        strcpy(
            categorias_idx
                .categorias_primario_idx[categorias_idx.qtd_registros_primario]
                .chave_primaria,
            c.id_curso);
        categorias_idx
            .categorias_primario_idx[categorias_idx.qtd_registros_primario]
            .proximo_indice = -1;
        categorias_idx.qtd_registros_primario++;

      } else { // se existir colocar na posição encontrada
        int id_curso =
            categorias_idx.categorias_secundario_idx[pos].primeiro_indice;
        while (
            categorias_idx.categorias_primario_idx[id_curso].proximo_indice !=
            -1)
          id_curso =
              categorias_idx.categorias_primario_idx[id_curso].proximo_indice;

        categorias_idx.categorias_primario_idx[id_curso].proximo_indice =
            categorias_idx.qtd_registros_primario;
        categorias_idx
            .categorias_primario_idx[categorias_idx.qtd_registros_primario]
            .proximo_indice = -1;
        strcpy(
            categorias_idx
                .categorias_primario_idx[categorias_idx.qtd_registros_primario]
                .chave_primaria,
            c.id_curso);
        categorias_idx.qtd_registros_primario++;
      }
    }
  }
  qsort(categorias_idx.categorias_secundario_idx,
        categorias_idx.qtd_registros_secundario,
        sizeof(categorias_secundario_index), qsort_categorias_secundario_idx);
  printf(INDICE_CRIADO, "categorias_idx");
}

/* Exibe um usuario dado seu RRN */
bool exibir_usuario(int rrn) {
  if (rrn < 0)
    return false;

  Usuario u = recuperar_registro_usuario(rrn);

  printf("%s, %s, %s, %s, %.2lf\n", u.id_usuario, u.nome, u.email, u.telefone,
         u.saldo);
  return true;
}

/* Exibe um curso dado seu RRN */
bool exibir_curso(int rrn) {
  if (rrn < 0)
    return false;

  Curso j = recuperar_registro_curso(rrn);

  printf("%s, %s, %s, %s, %s, %d, %.2lf\n", j.id_curso, j.titulo, j.instituicao,
         j.ministrante, j.lancamento, j.carga, j.valor);
  return true;
}

/* Exibe uma inscricao dado seu RRN */
bool exibir_inscricao(int rrn) {
  if (rrn < 0)
    return false;

  Inscricao c = recuperar_registro_inscricao(rrn);

  printf("%s, %s, %s, %c, %s\n", c.id_curso, c.id_usuario, c.data_inscricao,
         c.status, c.data_atualizacao);

  return true;
}

/* Recupera do arquivo de usuários o registro com o RRN
 * informado e retorna os dados na struct Usuario */
Usuario recuperar_registro_usuario(int rrn) {
  Usuario u;
  char temp[TAM_REGISTRO_USUARIO + 1], *p;
  strncpy(temp, ARQUIVO_USUARIOS + (rrn * TAM_REGISTRO_USUARIO),
          TAM_REGISTRO_USUARIO);
  temp[TAM_REGISTRO_USUARIO] = '\0';

  p = strtok(temp, ";");
  strcpy(u.id_usuario, p);
  p = strtok(NULL, ";");
  strcpy(u.nome, p);
  p = strtok(NULL, ";");
  strcpy(u.email, p);
  p = strtok(NULL, ";");
  strcpy(u.telefone, p);
  p = strtok(NULL, ";");
  u.saldo = atof(p);
  p = strtok(NULL, ";");

  return u;
}

/* Recupera do arquivo de cursos o registro com o RRN
 * informado e retorna os dados na struct Curso */
Curso recuperar_registro_curso(int rrn) {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  Curso c;
  char temp[TAM_REGISTRO_CURSO + 1], *p, *token_categorias;
  strncpy(temp, ARQUIVO_CURSOS + (rrn * TAM_REGISTRO_CURSO),
          TAM_REGISTRO_CURSO);
  temp[TAM_REGISTRO_CURSO] = '\0';

  p = strtok(temp, ";");
  strcpy(c.id_curso, p);
  p = strtok(NULL, ";");
  strcpy(c.titulo, p);
  p = strtok(NULL, ";");
  strcpy(c.instituicao, p);
  p = strtok(NULL, ";");
  strcpy(c.ministrante, p);
  p = strtok(NULL, ";");
  strcpy(c.lancamento, p);
  p = strtok(NULL, ";");
  c.carga = atoi(p);
  // strcpy(c.carga, p);
  p = strtok(NULL, ";");
  c.valor = atof(p);
  p = strtok(NULL, ";");
  // Aqui começa as categorias
  token_categorias = strtok(p, "|");

  for (int i = 0; i < QTD_MAX_CATEGORIAS; i++) {
    if (token_categorias) {
      strcpy(c.categorias[i], token_categorias);
      token_categorias = strtok(NULL, "|");
    }
  }

  return c;
  // printf(ERRO_NAO_IMPLEMENTADO, "recuperar_registro_curso");
}

/* Recupera do arquivo de inscricoes o registro com o RRN
 * informado e retorna os dados na struct Inscricao */
Inscricao recuperar_registro_inscricao(int rrn) {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  Inscricao i;
  char temp[TAM_REGISTRO_INSCRICAO + 1], *p;
  strncpy(temp, ARQUIVO_INSCRICOES + (rrn * TAM_REGISTRO_INSCRICAO),
          TAM_REGISTRO_INSCRICAO);
  temp[TAM_REGISTRO_INSCRICAO] = '\0';

  p = temp;
  strncpy(p, temp, 8);
  strcpy(i.id_curso, p);
  strncpy(p, temp + 8, 11);
  strcpy(i.id_usuario, p);
  strncpy(p, temp + 19, 12);
  strcpy(i.data_inscricao, p);
  strncpy(p, temp + 31, 1);
  strcpy(i.status, p);
  strncpy(p, temp + 32, 12);
  strcpy(i.data_atualizacao, p);

  return i;
  // printf(ERRO_NAO_IMPLEMENTADO, "recuperar_registro_inscricao");
}

/* Escreve no arquivo de usuários na posição informada (RRN)
 * os dados na struct Usuario */
void escrever_registro_usuario(Usuario u, int rrn) {
  char temp[TAM_REGISTRO_USUARIO + 1], p[100];
  temp[0] = '\0';
  p[0] = '\0';

  strcpy(temp, u.id_usuario);
  strcat(temp, ";");
  strcat(temp, u.nome);
  strcat(temp, ";");
  strcat(temp, u.email);
  strcat(temp, ";");
  strcat(temp, u.telefone);
  strcat(temp, ";");
  sprintf(p, "%013.2lf", u.saldo);
  strcat(temp, p);
  strcat(temp, ";");

  strpadright(temp, '#', TAM_REGISTRO_USUARIO);

  strncpy(ARQUIVO_USUARIOS + rrn * TAM_REGISTRO_USUARIO, temp,
          TAM_REGISTRO_USUARIO);
  ARQUIVO_USUARIOS[qtd_registros_usuarios * TAM_REGISTRO_USUARIO] = '\0';
}

/* Escreve no arquivo de cursos na posição informada (RRN)
 * os dados na struct Curso */
void escrever_registro_curso(Curso j, int rrn) {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  char temp[TAM_REGISTRO_CURSO + 1], p[100];
  temp[0] = '\0';
  p[0] = '\0';
  int tam_max = 256;
  char *t;
  strcpy(temp, j.id_curso);
  strcat(temp, ";");
  strcat(temp, j.titulo);
  strcat(temp, ";");
  strcat(temp, j.instituicao);
  strcat(temp, ";");
  strcat(temp, j.ministrante);
  strcat(temp, ";");
  strcat(temp, j.lancamento);
  strcat(temp, ";");
  sprintf(p, "%08d", j.carga);
  strcat(temp, p);
  strcat(temp, ";");
  sprintf(p, "%013.2lf", j.valor);
  strcat(temp, p);
  strcat(temp, ";");
  for (int i = 0; i < QTD_MAX_CATEGORIAS; i++) {
    if (strcmp(j.categorias[i], "") != 0) {
      strcpy(p, j.categorias[i]);
      strcat(temp, p);
      strcat(temp, "|");
    }
  }
  strcat(temp, ";");

  strpadright(temp, '#', TAM_REGISTRO_CURSO);

  strncpy(ARQUIVO_CURSOS + rrn * TAM_REGISTRO_CURSO, temp, TAM_REGISTRO_CURSO);
  ARQUIVO_CURSOS[qtd_registros_cursos * TAM_REGISTRO_CURSO] = '\0';
}

/* Escreve no arquivo de inscricoes na posição informada (RRN)
 * os dados na struct Inscricao */
void escrever_registro_inscricao(Inscricao c, int rrn) {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  char temp[TAM_REGISTRO_INSCRICAO + 1], p[100];
  temp[0] = p[0] = '\0';

  strcpy(temp, c.id_curso);
  strcat(temp, c.id_usuario);
  strcat(temp, c.data_inscricao);
  sprintf(p, "%c", c.status);
  strcat(temp, p);
  strcat(temp, c.data_atualizacao);

  strncpy(ARQUIVO_INSCRICOES + rrn * TAM_REGISTRO_INSCRICAO, temp,
          TAM_REGISTRO_INSCRICAO);
  ARQUIVO_INSCRICOES[qtd_registros_inscricoes * TAM_REGISTRO_INSCRICAO] = '\0';
  printf("%s\n", ARQUIVO_INSCRICOES);
}

/* Funções principais */
void cadastrar_usuario_menu(char *id_usuario, char *nome, char *email,
                            char *telefone) {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  Usuario new;
  // buscar por id_usuario igual
  // printf("lol");
  strcpy(new.id_usuario, id_usuario);
  strcpy(new.nome, nome);
  strcpy(new.email, email);
  // tratar sem entrada de telefone
  if (strcmp(telefone, "\0") != 0) {
    strcpy(new.telefone, telefone);
  } else {
    strcpy(new.telefone, "***********");
  }
  new.saldo = 0;
  // sprintf(0, "%013.2lf", new.saldo);

  // checar se o id_usuario já existe
  usuarios_index *koo =
      busca_binaria(new.id_usuario, usuarios_idx, qtd_registros_usuarios,
                    sizeof(usuarios_index), qsort_usuarios_idx, false, 0);
  if (koo) { // tratar id usuario igual porém rrn - 1, deve colocar na msm
             // posição TRATAR DOS INDICES KRL TODOS, BUSCA BINARIA PARAMETROS
    printf(ERRO_PK_REPETIDA, id_usuario);
    return;
  }
  strcpy(usuarios_idx[qtd_registros_usuarios].id_usuario, id_usuario);
  usuarios_idx[qtd_registros_usuarios].rrn = qtd_registros_usuarios;
  qtd_registros_usuarios++;
  qsort(usuarios_idx, qtd_registros_usuarios, sizeof(usuarios_index),
        qsort_usuarios_idx);
  escrever_registro_usuario(new, qtd_registros_usuarios - 1);
  printf(SUCESSO);
  // printf("%s\n", ARQUIVO_USUARIOS);
  //  printf(ERRO_NAO_IMPLEMENTADO, "cadastrar_usuario_menu");
}

void cadastrar_telefone_menu(char *id_usuario, char *telefone) {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  usuarios_index *koo =
      busca_binaria(id_usuario, usuarios_idx, qtd_registros_usuarios,
                    sizeof(usuarios_index), qsort_usuarios_idx, false, NULL);
  if (!koo) {
    printf(ERRO_REGISTRO_NAO_ENCONTRADO);
    return;
  }
  int rrn = koo->rrn;
  Usuario x = recuperar_registro_usuario(rrn);
  strcpy(x.telefone, telefone);
  escrever_registro_usuario(x, rrn);
  printf(SUCESSO);
  // printf(ERRO_NAO_IMPLEMENTADO, "cadastrar_telefone_menu");
}

void remover_usuario_menu(char *id_usuario) {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  usuarios_index *koo =
      busca_binaria(id_usuario, usuarios_idx, qtd_registros_usuarios,
                    sizeof(usuarios_index), qsort_usuarios_idx, false, NULL);
  if (!koo) {
    printf(ERRO_REGISTRO_NAO_ENCONTRADO);
    return;
  }
  int rrn = koo->rrn;
  Usuario x = recuperar_registro_usuario(rrn);
  x.id_usuario[0] = "*";
  x.id_usuario[1] = "|";
  koo->rrn = -1;
  escrever_registro_usuario(x, rrn);
  printf(SUCESSO);
  // printf(ERRO_NAO_IMPLEMENTADO, "remover_usuario_menu");
}

void cadastrar_curso_menu(char *titulo, char *instituicao, char *ministrante,
                          char *lancamento, int carga, double valor) {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */

  titulos_index *cursos =
      busca_binaria(titulo, titulo_idx, qtd_registros_cursos,
                    sizeof(titulos_index), qsort_titulo_idx, false, 0);
  if (cursos) {
    printf(ERRO_PK_REPETIDA, titulo);
    return;
  }
  Curso c;
  strcpy(c.titulo, titulo);
  strcpy(c.instituicao, instituicao);
  strcpy(c.ministrante, ministrante);
  strcpy(c.lancamento, lancamento);
  c.carga = carga;
  c.valor = valor;
  // char p[10];
  // p[0] = "\0";
  // sprintf(p, "%08d", qtd_registros_cursos);
  for (int i = 0; i < QTD_MAX_CATEGORIAS; i++) {
    strcpy(c.categorias[i], "");
  }
  sprintf(c.id_curso, "%08d", qtd_registros_cursos);

  strcpy(cursos_idx[qtd_registros_cursos].id_curso, c.id_curso);
  cursos_idx[qtd_registros_cursos].rrn = qtd_registros_cursos;

  strcpy(titulo_idx[qtd_registros_cursos].titulo, c.titulo);
  strcpy(titulo_idx[qtd_registros_cursos].id_curso, c.id_curso);

  qtd_registros_cursos++;
  qsort(cursos_idx, qtd_registros_cursos, sizeof(cursos_index),
        qsort_cursos_idx);
  qsort(titulo_idx, qtd_registros_cursos, sizeof(titulos_index),
        qsort_titulo_idx);
  escrever_registro_curso(c, qtd_registros_cursos - 1);
  printf(SUCESSO);
}

void adicionar_saldo_menu(char *id_usuario, double valor) {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  usuarios_index *user =
      busca_binaria(id_usuario, usuarios_idx, qtd_registros_usuarios,
                    sizeof(usuarios_index), qsort_usuarios_idx, false, 0);
  if (!user) {
    printf(ERRO_REGISTRO_NAO_ENCONTRADO);
    return;
  }
  if (valor <= 0) {
    printf(ERRO_VALOR_INVALIDO);
    return;
  }
  int rrn = user->rrn;
  Usuario x = recuperar_registro_usuario(rrn);
  x.saldo += valor;
  escrever_registro_usuario(x, rrn);
  printf(SUCESSO);
  // printf(ERRO_NAO_IMPLEMENTADO, "adicionar_saldo_menu");
}

void inscrever_menu(char *id_curso, char *id_usuario) {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  // printf("%s\n", ARQUIVO_CURSOS);
  inscricoes_index igual;
  strcpy(igual.id_curso, id_curso);
  strcpy(igual.id_usuario, id_usuario);
  // printf("%s", ARQUIVO_CURSOS);
  // printf("%d\n", strcmp(id_curso, cursos_idx[0].id_curso));
  cursos_index *c =
      busca_binaria(id_curso, cursos_idx, qtd_registros_cursos,
                    sizeof(cursos_index), qsort_cursos_idx, false, 0);
  if (!c) {
    printf(ERRO_REGISTRO_NAO_ENCONTRADO);
    return;
  }
  int rrn_curso = c->rrn;

  usuarios_index *u =
      busca_binaria(id_usuario, usuarios_idx, qtd_registros_usuarios,
                    sizeof(usuarios_index), qsort_usuarios_idx, false, 0);
  if (!u) {
    printf(ERRO_REGISTRO_NAO_ENCONTRADO);
    return;
  }
  int rrn_usuario = u->rrn;
  // incrição a buscar obs: busquei na internet sobre como buscar multiplos
  // parametros pela mesma chave
  inscricoes_index buscar;

  strcpy(buscar.id_curso, id_curso);
  strcpy(buscar.id_usuario, id_usuario);
  inscricoes_index *i =
      busca_binaria(&buscar, inscricoes_idx, qtd_registros_inscricoes,
                    sizeof(inscricoes_index), qsort_inscricoes_idx, false, 0);
  if (i) {
    printf(ERRO_PK_REPETIDA, i->id_curso);
    return;
  }

  Curso curso = recuperar_registro_curso(rrn_curso);
  Usuario user = recuperar_registro_usuario(rrn_usuario);

  if (user.saldo < curso.valor) {
    printf(ERRO_SALDO_NAO_SUFICIENTE);
    return;
  }
  char data_atual[TAM_DATETIME + 1];
  current_datetime(data_atual);
  Inscricao inscricao;
  strcpy(inscricao.id_curso, id_curso);
  strcpy(inscricao.id_usuario, id_usuario);
  strcpy(inscricao.data_inscricao, data_atual);
  strcpy(inscricao.data_atualizacao, data_atual);
  // strcpy(inscricao.status, "A");
  // strcpy(&inscricao.status, "A");
  inscricao.status = 'A';
  printf(SUCESSO);
  strcpy(inscricoes_idx[qtd_registros_inscricoes].id_curso, id_curso);
  strcpy(inscricoes_idx[qtd_registros_inscricoes].id_usuario, id_usuario);
  inscricoes_idx[qtd_registros_inscricoes].rrn = qtd_registros_inscricoes;
  qtd_registros_inscricoes++;
  qsort(inscricoes_idx, qtd_registros_inscricoes, sizeof(inscricoes_index),
        qsort_inscricoes_idx);
  escrever_registro_inscricao(inscricao, qtd_registros_inscricoes - 1);
}

void cadastrar_categoria_menu(char *titulo, char *categoria) {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  cursos_index *c =
      busca_binaria(titulo, cursos_idx, qtd_registros_cursos,
                    sizeof(cursos_index), qsort_cursos_idx, false, 0);
  if (!c) {
    printf(ERRO_REGISTRO_NAO_ENCONTRADO);
    return;
  }
  int rrn = c->rrn;
  Curso curso = recuperar_registro_curso(rrn);
  for (int i = 0; i < QTD_MAX_CATEGORIAS; i++) {
    if (strcmp(curso.categorias[i], categoria) == 0) {
      printf(ERRO_CATEGORIA_REPETIDA, curso.titulo, categoria);
      return;
    } else if (strcmp(curso.categorias[i], "") == 0) {
      strcpy(curso.categorias[i], categoria);
      escrever_registro_curso(curso, rrn);
      printf(SUCESSO);
      return;
    }
  }
}

void atualizar_status_inscricoes_menu(char *id_usuario, char *titulo,
                                      char status) {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  Inscricao buscar;

  strcpy(buscar.id_usuario, id_usuario);
  strcpy(buscar.id_curso, titulo);
  inscricoes_index *i =
      busca_binaria(&buscar, inscricoes_idx, qtd_registros_inscricoes,
                    sizeof(inscricoes_index), qsort_inscricoes_idx, false, 0);
  if (!i) {
    printf(ERRO_REGISTRO_NAO_ENCONTRADO);
    return;
  }
  int rrn = i->rrn;
  Inscricao inscricao = recuperar_registro_inscricao(rrn);
  inscricao.status = status;
  // strcpy(inscricao.status, status);
  escrever_registro_inscricao(inscricao, rrn);
  printf(SUCESSO);
  // printf(ERRO_NAO_IMPLEMENTADO, "atualizar_status_inscricoes_menu");
}

/* Busca */
void buscar_usuario_id_menu(char *id_usuario) {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  usuarios_index *buscar =
      busca_binaria(id_usuario, usuarios_idx, qtd_registros_usuarios,
                    sizeof(usuarios_index), qsort_usuarios_idx, true, 0);
  if (!buscar) {
    printf(ERRO_REGISTRO_NAO_ENCONTRADO);
    return;
  }
  exibir_usuario(buscar->rrn);

  // printf(ERRO_NAO_IMPLEMENTADO, "buscar_usuario_id_menu");
}

void buscar_curso_id_menu(char *id_curso) {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  cursos_index *buscar =
      busca_binaria(id_curso, cursos_idx, qtd_registros_cursos,
                    sizeof(cursos_index), qsort_cursos_idx, true, 0);
  if (!buscar) {
    printf(ERRO_REGISTRO_NAO_ENCONTRADO);
    return;
  }
  exibir_curso(buscar->rrn);

  // printf(ERRO_NAO_IMPLEMENTADO, "buscar_curso_id_menu");
}

void buscar_curso_titulo_menu(char *titulo) {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */

  cursos_index *buscar =
      busca_binaria(titulo, cursos_idx, qtd_registros_cursos,
                    sizeof(cursos_index), qsort_cursos_idx, true, 0);
  if (!buscar) {
    printf(ERRO_REGISTRO_NAO_ENCONTRADO);
    return;
  }
  exibir_curso(buscar->rrn);
  // printf(ERRO_NAO_IMPLEMENTADO, "buscar_curso_titulo_menu");
}

/* Listagem */
void listar_usuarios_id_menu() {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  bool sem_usuarios = true;
  for (int i = 0; i < qtd_registros_usuarios; i++) {
    if (usuarios_idx[i].rrn != -1) {
      sem_usuarios = false;
      exibir_usuario(usuarios_idx[i].rrn);
    }
  }
  if (sem_usuarios) {
    printf(AVISO_NENHUM_REGISTRO_ENCONTRADO);
  }
  // printf(ERRO_NAO_IMPLEMENTADO, "listar_usuarios_id_menu");
}

void listar_cursos_categorias_menu(char *categoria) {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  imprimir_categorias_primario_idx_menu();
  categorias_secundario_index *buscar =
      busca_binaria(categoria, categorias_idx.categorias_secundario_idx,
                    categorias_idx.qtd_registros_secundario,
                    sizeof(categorias_secundario_index),
                    qsort_categorias_secundario_idx, false, 0);
  if (!buscar) {
    printf(AVISO_NENHUM_REGISTRO_ENCONTRADO);
    return;
  }
  int registro = buscar->primeiro_indice;
  while (registro != -1) {
    exibir_curso(registro);
    registro = categorias_idx.categorias_primario_idx[registro].proximo_indice;
  }
  // printf(ERRO_NAO_IMPLEMENTADO, "listar_cursos_categorias_menu");
}

void listar_inscricoes_periodo_menu(char *data_inicio, char *data_fim) {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  // data_curso_usuario_index *buscar = busca_binaria(data_inicio,
  // data_curso_usuario_idx, qtd_registros_inscricoes,
  // sizeof(data_curso_usuario_index), qsort_data_idx, true, NULL); if(!buscar){
  //   printf(AVISO_NENHUM_REGISTRO_ENCONTRADO);
  //   return;
  // }
  // do
  // {
  //   exibir_inscricao(buscar.);
  //   buscar++;
  // } while (strcmp(buscar->data, data_fim) <= 0);

  // printf(ERRO_NAO_IMPLEMENTADO, "listar_inscricoes_periodo_menu");
}

/* Liberar espaço */
void liberar_espaco_menu() {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  // printf(ERRO_NAO_IMPLEMENTADO, "liberar_espaco_menu");
}

/* Imprimir arquivos de dados */
void imprimir_arquivo_usuarios_menu() {
  if (qtd_registros_usuarios == 0)
    printf(ERRO_ARQUIVO_VAZIO);
  else
    printf("%s\n", ARQUIVO_USUARIOS);
}

void imprimir_arquivo_cursos_menu() {
  if (qtd_registros_cursos == 0)
    printf(ERRO_ARQUIVO_VAZIO);
  else
    printf("%s\n", ARQUIVO_CURSOS);
}

void imprimir_arquivo_inscricoes_menu() {
  if (qtd_registros_inscricoes == 0)
    printf(ERRO_ARQUIVO_VAZIO);
  else
    printf("%s\n", ARQUIVO_INSCRICOES);
}

/* Imprimir índices primários */
void imprimir_usuarios_idx_menu() {
  if (usuarios_idx == NULL || qtd_registros_usuarios == 0)
    printf(ERRO_ARQUIVO_VAZIO);
  else
    for (unsigned i = 0; i < qtd_registros_usuarios; ++i)
      printf("%s, %d\n", usuarios_idx[i].id_usuario, usuarios_idx[i].rrn);
}

void imprimir_cursos_idx_menu() {
  if (cursos_idx == NULL || qtd_registros_cursos == 0)
    printf(ERRO_ARQUIVO_VAZIO);
  else
    for (unsigned i = 0; i < qtd_registros_cursos; ++i)
      printf("%s, %d\n", cursos_idx[i].id_curso, cursos_idx[i].rrn);
}

void imprimir_inscricoes_idx_menu() {
  if (inscricoes_idx == NULL || qtd_registros_inscricoes == 0)
    printf(ERRO_ARQUIVO_VAZIO);
  else
    for (unsigned i = 0; i < qtd_registros_inscricoes; ++i)
      printf("%s, %s, %d\n", inscricoes_idx[i].id_curso,
             inscricoes_idx[i].id_usuario, inscricoes_idx[i].rrn);
}

/* Imprimir índices secundários */
void imprimir_titulo_idx_menu() {
  if (titulo_idx == NULL || qtd_registros_cursos == 0)
    printf(ERRO_ARQUIVO_VAZIO);
  else
    for (unsigned i = 0; i < qtd_registros_cursos; ++i)
      printf("%s, %s\n", titulo_idx[i].titulo, titulo_idx[i].id_curso);
}

void imprimir_data_curso_usuario_idx_menu() {
  if (data_curso_usuario_idx == NULL || qtd_registros_inscricoes == 0)
    printf(ERRO_ARQUIVO_VAZIO);
  else
    for (unsigned i = 0; i < qtd_registros_inscricoes; ++i)
      printf("%s, %s, %s\n", data_curso_usuario_idx[i].data,
             data_curso_usuario_idx[i].id_curso,
             data_curso_usuario_idx[i].id_usuario);
}

void imprimir_categorias_secundario_idx_menu() {
  if (categorias_idx.categorias_secundario_idx == NULL ||
      categorias_idx.qtd_registros_secundario == 0)
    printf(ERRO_ARQUIVO_VAZIO);
  else
    for (unsigned i = 0; i < categorias_idx.qtd_registros_secundario; ++i)
      printf("%s, %d\n",
             (categorias_idx.categorias_secundario_idx)[i].chave_secundaria,
             (categorias_idx.categorias_secundario_idx)[i].primeiro_indice);
}

void imprimir_categorias_primario_idx_menu() {
  if (categorias_idx.categorias_primario_idx == NULL ||
      categorias_idx.qtd_registros_primario == 0)
    printf(ERRO_ARQUIVO_VAZIO);
  else
    for (unsigned i = 0; i < categorias_idx.qtd_registros_primario; ++i)
      printf("%s, %d\n",
             (categorias_idx.categorias_primario_idx)[i].chave_primaria,
             (categorias_idx.categorias_primario_idx)[i].proximo_indice);
}

/* Liberar memória e encerrar programa */
void liberar_memoria_menu() {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  free(usuarios_idx);
  free(cursos_idx);
  free(inscricoes_idx);
  free(titulo_idx);
  free(data_curso_usuario_idx);
  free(categorias_idx.categorias_primario_idx);
  free(categorias_idx.categorias_secundario_idx);
}

/* Função de comparação entre chaves do índice usuarios_idx */
int qsort_usuarios_idx(const void *a, const void *b) {
  return strcmp(((usuarios_index *)a)->id_usuario,
                ((usuarios_index *)b)->id_usuario);
}

/* Função de comparação entre chaves do índice cursos_idx */
int qsort_cursos_idx(const void *a, const void *b) {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  return strcmp(((cursos_index *)a)->id_curso, ((cursos_index *)b)->id_curso);
  // printf(ERRO_NAO_IMPLEMENTADO, "qsort_cursos_idx");
}

/* Função de comparação entre chaves do índice inscricoes_idx */
int qsort_inscricoes_idx(const void *a, const void *b) {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */

  if (strcmp(((inscricoes_index *)a)->id_curso,
             ((inscricoes_index *)b)->id_curso) == 0)
    return strcmp(((inscricoes_index *)a)->id_usuario,
                  ((inscricoes_index *)b)->id_usuario);
  else
    return strcmp(((inscricoes_index *)a)->id_curso,
                  ((inscricoes_index *)b)->id_curso);
  // printf(ERRO_NAO_IMPLEMENTADO, "qsort_inscricoes_idx");
}

/* Função de comparação entre chaves do índice titulo_idx */
int qsort_titulo_idx(const void *a, const void *b) {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  return strcmp(((titulos_index *)a)->titulo, ((titulos_index *)b)->titulo);
  // printf(ERRO_NAO_IMPLEMENTADO, "qsort_titulo_idx");
}

/* Funções de comparação entre chaves do índice data_curso_usuario_idx */
int qsort_data_idx(const void *a, const void *b) {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  int data_cronologica = strcmp(((data_curso_usuario_index *)a)->data,
                                ((data_curso_usuario_index *)b)->data);
  if (data_cronologica > 0)
    return 0;
  else
    return data_cronologica;
  // printf(ERRO_NAO_IMPLEMENTADO, "qsort_data_idx");
}

int qsort_data_curso_usuario_idx(const void *a, const void *b) {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  int ordem_data = strcmp(((data_curso_usuario_index *)a)->data,
                          ((data_curso_usuario_index *)b)->data);
  if (ordem_data == 0) { // igual
    int ordem_curso = strcmp(((data_curso_usuario_index *)a)->id_curso,
                             ((data_curso_usuario_index *)b)->id_curso);
    if (ordem_curso == 0) {
      return strcmp(((data_curso_usuario_index *)a)->id_usuario,
                    ((data_curso_usuario_index *)b)->id_usuario);
    } else {
      return ordem_curso;
    }
  } else {
    return ordem_data;
  }
  // if (strcmp(((data_curso_usuario_index *)a)->id_curso,
  //            ((data_curso_usuario_index *)b)->id_curso) < 0) {
  //   if (strcmp(((data_curso_usuario_index *)a)->id_usuario,
  //              ((data_curso_usuario_index *)b)->id_usuario) < 0) {
  //     if (strcmp(((data_curso_usuario_index *)a)->data,
  //                ((data_curso_usuario_index *)b)->data) < 0) {
  //       return -1;
  //     } else {
  //       return 1;
  //     }
  //     return 0;
  //   } else {
  //     return 1;
  //   }
  // } else {
  //   return 1;
  // }
  // printf(ERRO_NAO_IMPLEMENTADO, "qsort_data_curso_usuario_idx");
}

/* Função de comparação entre chaves do índice secundário de categorias_idx */
int qsort_categorias_secundario_idx(const void *a, const void *b) {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  return strcmp(((categorias_secundario_index *)a)->chave_secundaria,
                ((categorias_secundario_index *)b)->chave_secundaria);
  // printf(ERRO_NAO_IMPLEMENTADO, "qsort_categorias_secundario_idx");
}

/* Funções de manipulação de Lista Invertida */
void inverted_list_insert(char *chave_secundaria, char *chave_primaria,
                          inverted_list *t) {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  char *p;
  strcpy(p, chave_secundaria);
  // checar se a categoria já existe e pegar a posição caso exista
  bool existe = false;
  int pos;
  for (int j = 0; j < t->qtd_registros_secundario; j++) {
    if (strcmp(t->categorias_secundario_idx[j].chave_secundaria, p) == 0) {
      existe = true;
      pos = j;
      break;
    }
  }
  if (!existe) { // se não existe colocar na ultima posição como categoria
                 // nova
    // lido com a chave secundária
    strcpy(t->categorias_secundario_idx[t->qtd_registros_secundario]
               .chave_secundaria,
           p);
    t->categorias_secundario_idx[t->qtd_registros_secundario].primeiro_indice =
        t->qtd_registros_primario;
    t->qtd_registros_secundario++;

    // lido com a chave primária
    strcpy(t->categorias_primario_idx[t->qtd_registros_primario].chave_primaria,
           chave_primaria);
    t->categorias_primario_idx[t->qtd_registros_primario].proximo_indice = -1;
    t->qtd_registros_primario++;

  } else { // se existir colocar na posição encontrada
    int id_curso = t->categorias_secundario_idx[pos].primeiro_indice;
    while (t->categorias_primario_idx[id_curso].proximo_indice != -1)
      id_curso = t->categorias_primario_idx[id_curso].proximo_indice;

    t->categorias_primario_idx[id_curso].proximo_indice =
        t->qtd_registros_primario;
    t->categorias_primario_idx[t->qtd_registros_primario].proximo_indice = -1;
    strcpy(t->categorias_primario_idx[t->qtd_registros_primario].chave_primaria,
           chave_primaria);
    t->qtd_registros_primario++;
  }

  qsort(t->categorias_secundario_idx, t->qtd_registros_secundario,
        sizeof(categorias_secundario_index), qsort_categorias_secundario_idx);
  // printf(ERRO_NAO_IMPLEMENTADO, "inverted_list_insert");
}

bool inverted_list_secondary_search(int *result, bool exibir_caminho,
                                    char *chave_secundaria, inverted_list *t) {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  categorias_secundario_index *a =
      busca_binaria(t->categorias_secundario_idx, t->qtd_registros_secundario,
                    sizeof(categorias_secundario_index), chave_secundaria,
                    qsort_categorias_secundario_idx, exibir_caminho, -1);
  if (a == -1) {
    return false;
  } else { // buscar todos os ids de curso com categorias
    int id_curso = a->primeiro_indice;
    int i = 0;
    while (id_curso != -1) {
      result[i] = id_curso;
      id_curso = t->categorias_primario_idx[id_curso].proximo_indice;
      i++;
    }
    return true;
  }
  // printf(ERRO_NAO_IMPLEMENTADO, "inverted_list_secondary_search");
}

int inverted_list_primary_search(
    char result[][TAM_CHAVE_CATEGORIAS_PRIMARIO_IDX], bool exibir_caminho,
    int indice, int *indice_final, inverted_list *t) {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  int cursos = 0;
  int indice_retornado = indice;
  while (t->categorias_primario_idx[indice].proximo_indice != -1) {
    strcpy(result[cursos], t->categorias_primario_idx[indice].chave_primaria);
    indice = t->categorias_primario_idx[indice].proximo_indice;
    cursos++;
    indice_final = indice;
  }
  return cursos;

  printf(ERRO_NAO_IMPLEMENTADO, "inverted_list_primary_search");
}

char *strpadright(char *str, char pad, unsigned size) {
  for (unsigned i = strlen(str); i < size; ++i)
    str[i] = pad;
  str[size] = '\0';
  return str;
}

char *strupr(char *str) {
  for (char *p = str; *p; ++p)
    *p = toupper(*p);
  return str;
}

char *strlower(char *str) {
  for (char *p = str; *p; ++p)
    *p = tolower(*p);
  return str;
}

/* Funções da busca binária */
/* Função baseada na biblioteca GNU */
void *busca_binaria(const void *key, const void *base0, size_t nmemb,
                    size_t size, int (*compar)(const void *, const void *),
                    bool exibir_caminho, int retorno_se_nao_encontrado) {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  int start = 0;
  int end = nmemb - 1;
  int middle;
  if (exibir_caminho) {
  }
  // bool par = nmemb % 2;
  while (start <= end) {
    // printf("%d %d\n", (int)start, (int)end);

    // middle = (start + end + 1) / 2;
    middle = start + ((end - start + 1) / 2);

    const void *base = (char *)base0 + middle * size;
    // printf("id : %s\nstart: %d, end: %d\nelemtos %d\n",
    //       ((usuarios_index *)base)->id_usuario, (int)start, (int)end),
    //    (int)nmemb;
    // printf("%s", ((usuarios_index *)base)->id_usuario);
    int result = compar(key, base);
    // printf("result : %d\n", result);
    if (result == 0) { // achou
      return (void *)base;
    } else if (result < 0) {
      end = middle - 1;
    } else {
      start = middle + 1;
    }
    if (exibir_caminho) {
      printf("%d ", (int)middle);
    }
  }
  if (retorno_se_nao_encontrado == 0) {
    return NULL;
  }
  // return (void *)retorno_se_nao_encontrado;
  //  printf(ERRO_NAO_IMPLEMENTADO, "busca_binaria");
}
