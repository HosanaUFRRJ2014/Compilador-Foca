#include<iostream>
#include <string>
#include <sstream>
#include <map>
#include <utility>

using namespace std;

namespace ControleDeVariaveis
{
	struct DADOS_VARIAVEL
	{
		string tipo; //Tipo da variável.
		string nome; //Nome que o usuário deu a variável no programa.
		string nomeTraducao; //Nome da variável no código intermediário.
		int tamanho = 0; //Para string
		bool ehDinamica = false; //Para string
		string labelTamanhoDinamicoString;
		int escopo;
		vector<string> pilhaTamanhoDimensoesArray; //Para arrays -> Guarda as dimensões do array. Para obter a qtd de dim, só usar o size()
		string tipoArray; //Para arrays -> Tipo primitivo do Array.
		vector<pair<string,bool>> valoresReaisDim; //Para arrays ---> Para tratamente em tempo de compilação dos erros da indexação(TK_NUM).
		bool foiCriadoDinamicamente;
	};


	namespace MapaDeContexto
	{
		//define de prefixo do usuario movido para o sintatica para que todos os arquivos tenham acesso
//		#define prefixo_funcao_usuario "FUNCUSER_"
//		#define constante_TamanhoInicialPilha 5
		int numeroEscopoAtual = 0;
		vector<map<string, DADOS_VARIAVEL>*> pilhaDeMapas(0);
		map<string, DADOS_VARIAVEL> *mapaDeContexto;
		map<string, string> dicionarioNomeTraducaoParaNome;

		void inicializarMapaDeContexto();
		bool incluirNoMapa(string,int, string,string,vector< pair<string,bool>>,vector<string>);
		bool atualizarNoMapa(DADOS_VARIAVEL, int escopo = numeroEscopoAtual);
		bool atualizarLabelTamanhoDinamicoNoMapa(string, string, int escopo = numeroEscopoAtual);
		string recuperarLabelTamanhoDinamicoString(string , int escopoDeAcesso = numeroEscopoAtual);
		bool variavelJaDeclarada(string, bool varrerEscopo = true, int escopo = numeroEscopoAtual);
		DADOS_VARIAVEL recuperarDadosVariavel(string, int escopo = numeroEscopoAtual);
		string gerarNomeTraducaoVariavelUsuario();
		string recuperarNomeTraducao(string nome, int escopo = numeroEscopoAtual);
		string recuperarNome(string nomeTraducao, int escopo = numeroEscopoAtual);


		//escopo
		void aumentarEscopo();
		void empilha(map<string, DADOS_VARIAVEL>*);
		void diminuirEscopo();
		bool ehMaiorIgualQueEscopoAtual(int);
		int escopoResultante(int);

		//Impressao e debugs
		void imprimirVarDados(DADOS_VARIAVEL);

		void imprimirVarDados(DADOS_VARIAVEL metaData){
			cout << "Nome: " << metaData.nome << endl << "Nome Traducao: " << metaData.nomeTraducao << endl << "Tipo: " << metaData.tipo << endl;
			cout << "Tam pilhaDimArray: " << metaData.pilhaTamanhoDimensoesArray.size() << endl << "Tam valoresReaisDim: " << metaData.valoresReaisDim.size() << endl;
			cout << "Tipo Array: " << metaData.tipoArray << endl;
		}


		bool ehMaiorIgualQueEscopoAtual(int qtdRetornoEscopo)
		{
			return qtdRetornoEscopo >= numeroEscopoAtual;
		}

		int escopoResultante(int qtdRetornoEscopo)
		{
			return numeroEscopoAtual - qtdRetornoEscopo;
		}


		void inicializarMapaDeContexto()
		{
			//map<string, DADOS_VARIAVEL> *mapa = (map<string, DADOS_VARIAVEL>*) malloc(sizeof(map<string, DADOS_VARIAVEL>));
			//mapa->clear();
			map<string, DADOS_VARIAVEL> *mapa = new map<string, DADOS_VARIAVEL>();
			pilhaDeMapas.push_back(mapa);
		}

		void aumentarEscopo(){
			//map<string, DADOS_VARIAVEL> *novoMapa = (map<string, DADOS_VARIAVEL>*) malloc(sizeof(map<string, DADOS_VARIAVEL>));
			map<string, DADOS_VARIAVEL>* novoMapa = new map<string, DADOS_VARIAVEL>();

			DADOS_VARIAVEL abc;

			numeroEscopoAtual = numeroEscopoAtual+1;
//			cout << novoMapa->size();
//			novoMapa->clear();
			pilhaDeMapas.push_back(novoMapa);

		}

		void diminuirEscopo(){
			numeroEscopoAtual = numeroEscopoAtual-1;
			pilhaDeMapas.pop_back();//desempilha

		}

		string adicionaPrefixo(string nome)
		{
			if(nome.find(prefixo_variavel_usuario) != 0)
				nome = prefixo_variavel_usuario + nome;
			return nome;
		}

		bool incluirNoMapa(string nome, int tamanho, string tipo = "",string tipoArray = "",vector<pair<string,bool>> valorReal = {},vector<string> dim = {}, bool foiCriadoDinamicamente = false)
		{
			nome = adicionaPrefixo(nome);
			if(!variavelJaDeclarada(nome, false))
			{
				DADOS_VARIAVEL variavel;
				variavel.nome = nome;
				variavel.tamanho = tamanho;
				variavel.tipo = tipo;
				variavel.escopo = numeroEscopoAtual;
				variavel.nomeTraducao = gerarNomeTraducaoVariavelUsuario();
				variavel.pilhaTamanhoDimensoesArray = dim; //Para o Array.
				variavel.tipoArray = tipoArray; //Para o Array.
				variavel.valoresReaisDim = valorReal; //Para o Array.
				variavel.foiCriadoDinamicamente = foiCriadoDinamicamente; //Para o Array
				dicionarioNomeTraducaoParaNome.insert(pair<string, string>(variavel.nomeTraducao, nome));
				pilhaDeMapas[numeroEscopoAtual]->insert(pair<string,DADOS_VARIAVEL>(nome,variavel));

				return true;
			}

			return false;
		}

		bool atualizarNoMapa(DADOS_VARIAVEL variavel, int escopo)
		{

			if(variavelJaDeclarada(variavel.nome, true, escopo))
			{
				mapaDeContexto->at(variavel.nome).tamanho = variavel.tamanho;
				mapaDeContexto->at(variavel.nome).ehDinamica = variavel.ehDinamica;
				mapaDeContexto->at(variavel.nome).labelTamanhoDinamicoString = variavel.labelTamanhoDinamicoString;
				mapaDeContexto->at(variavel.nome).pilhaTamanhoDimensoesArray = variavel.pilhaTamanhoDimensoesArray;
				mapaDeContexto->at(variavel.nome).valoresReaisDim = variavel.valoresReaisDim;
				mapaDeContexto->at(variavel.nome).tipoArray = variavel.tipoArray;
				mapaDeContexto->at(variavel.nome).foiCriadoDinamicamente = variavel.foiCriadoDinamicamente;
				if(mapaDeContexto->at(variavel.nome).tipo == "")
				{
					mapaDeContexto->at(variavel.nome).tipo = variavel.tipo;
					return true;
				}
				else
				{
					if(mapaDeContexto->at(variavel.nome).tipo == constante_tipo_funcao)
					{
						mapaDeContexto->at(variavel.nome).nomeTraducao = variavel.nomeTraducao;
					}
				}

				/*if(mapaDeContexto->at(variavel.nome).tipo == constante_tipo_string){
					mapaDeContexto->at(variavel.nome).ehDinamica = variavel.ehDinamica;
				}*/
			}
			return false;
		}

		bool atualizarLabelTamanhoDinamicoNoMapa(string labelNoMapa, string labelTamanhoDinamicoString, int escopo)
		{
			if(variavelJaDeclarada(labelNoMapa, true, escopo))
			{
				mapaDeContexto->at(labelNoMapa).labelTamanhoDinamicoString = labelTamanhoDinamicoString;
				return true;
			}

			return false;
		}

		string recuperarLabelTamanhoDinamicoString(string label, int escopo)
		{
			if(variavelJaDeclarada(label, true, escopo))
				 return mapaDeContexto->at(label).labelTamanhoDinamicoString;

		}

		bool variavelJaDeclarada(string nome, bool varrerEscopo, int escopo)
		{
			//ponto de parada
			if(escopo < 0)
				return false;
			//variavel que vai manter qual foi o ultimo mapa acessado
			mapaDeContexto = pilhaDeMapas[escopo];
			if(varrerEscopo)
			{
				nome = adicionaPrefixo(nome);

				//operador curto circuitado para buscar a variavel nos mapas recursivamente
				return (mapaDeContexto->count(nome) > 0) || variavelJaDeclarada(nome, varrerEscopo, escopo - 1);
			}
			else
			{
				nome = adicionaPrefixo(nome);
				return mapaDeContexto->count(nome) > 0;
			}
		}

		DADOS_VARIAVEL recuperarDadosVariavel(string nome, int escopo)
		{
			DADOS_VARIAVEL retorno;
			nome = adicionaPrefixo(nome);
			if(variavelJaDeclarada(nome, true, escopo))
			{
				 return mapaDeContexto->at(nome);
			}

			return retorno;
		}

		string gerarNomeTraducaoVariavelUsuario(){
			static int num = 0;
			num++;
			string temp = "variavelDoUsuario";

			string numInt = to_string(num);
			return temp + numInt;
		}

		string recuperarNomeTraducao(string nome, int escopo){
			if(escopo == -1)
				escopo = numeroEscopoAtual;
			if(variavelJaDeclarada(nome, true, escopo))
			{
				return recuperarDadosVariavel(nome, escopo).nomeTraducao;
			}
			return nome;
		}

		string recuperarNome(string nomeTraducao, int escopo)
		{
			//operador curto circuitado para buscar a variavel nos mapas recursivamente
			return dicionarioNomeTraducaoParaNome.at(nomeTraducao);
		}

	}


		namespace ControleSubstituicaoDeVariaveis
		{
				int calcularTamanhoSubstituicao(int , string );

		    int calcularTamanhoSubstituicao(int posInicial, string sustituivel)
		    {
		        int i = posInicial;
		        int tamSubstituicao = 0;
		        while (sustituivel[i] != ';')
		        {
		            tamSubstituicao++;
		            i++;
		        }
		        return tamSubstituicao;

		    }

		}

	namespace DeclaracaoProvisoriaInferenciaTipo
	{
		using namespace MapaDeContexto;
		using namespace ControleSubstituicaoDeVariaveis;

		#define constante_subst_tipo_declaracao_variavel "//#TIPOP_VAR_\t_\t#"
		#define constante_sufixo_escopo "SCOPE"
		#define slotIdVar "\t"
		map<string, string> mapaSubstituicaoDeTipoProvisorio;
		string construirDeclaracaoProvisoriaDeInferenciaDeTipo(string);

		void adicionarDefinicaoDeTipo(string, string, int,bool, int,string);
		string substituirTodasAsDeclaracoesProvisorias(string);
		string montarTagTipoProvisorio(string, int);
		string recuperarIdPelaTag(string);

		string montarTagTipoProvisorio(string id, int escopo){
			string constanteMarcacao = constante_subst_tipo_declaracao_variavel;
			string separador = slotIdVar;
			string idPrefixado = adicionaPrefixo(id);
			//primeiro insere o numero do escopo da variavel no primeiro slot e depois adiciona o nome da variavel no segundo slot
			string sufixoEscopo = constante_sufixo_escopo;
			string tipoProvisorio = constanteMarcacao.replace(constanteMarcacao.find(separador), separador.length(), sufixoEscopo + to_string(escopo));
			tipoProvisorio = tipoProvisorio.replace(tipoProvisorio.find(separador), separador.length(), idPrefixado);
			return tipoProvisorio;
		}

		string construirDeclaracaoProvisoriaDeInferenciaDeTipo(string id)
		{
		/*
			string constanteMarcacao = constante_subst_tipo_declaracao_variavel;
			string separador = slotIdVar;
			string idPrefixado = adicionaPrefixo(id);
			//primeiro insere o nome da variavel no primeiro slot e depois adiciona o numero do escopo no segundo slot
			string tipoProvisorio = constanteMarcacao.replace(constanteMarcacao.find(separador), separador.length(), idPrefixado);
			string sufixoEscopo = constante_sufixo_escopo;
			tipoProvisorio = tipoProvisorio.replace(tipoProvisorio.find(separador), separador.length(), sufixoEscopo + to_string(numeroEscopoAtual));
			*/
			//string tipoProvisorio = montarTagTipoProvisorio(id, numeroEscopoAtual);

			id = adicionaPrefixo(id);
			string tipoProvisorio = recuperarNomeTraducao(id, numeroEscopoAtual);
			mapaSubstituicaoDeTipoProvisorio[tipoProvisorio] = " ";
			return tipoProvisorio + " " + tipoProvisorio + "; //" + id + "\n";
		}


		//void adicionarDefinicaoDeTipo(string id, string tipo, int tamanho,  int escopo = numeroEscopoAtual)
		void adicionarDefinicaoDeTipo(string id, string tipo, int tamanho, bool ehDinamica, int escopo = numeroEscopoAtual, string tipoArray = "")
		{
			/*string constanteMarcacao = constante_subst_tipo_declaracao_variavel;
			string idPrefixado = adicionaPrefixo(id);
			string separador = slotIdVar;
			string tipoProvisorio = constanteMarcacao.replace(constanteMarcacao.find(separador), separador.length(), idPrefixado);
			string sufixoEscopo = constante_sufixo_escopo;
			tipoProvisorio = tipoProvisorio.replace(tipoProvisorio.find(separador), separador.length(), sufixoEscopo + to_string(escopo));*/
			id = adicionaPrefixo(id);

			string tipoProvisorio = recuperarNomeTraducao(id, escopo);

			//verificação a mais inserida pq havia problema na hora de definir o tipo de uma variavel global
			//pq o escopo da variavel global é 0 mas o C++ só aceita definição de valor de variavel global em algum escopo interno
			//então na hora de substituir o escopo é 1, mas deveria ser 0 ... então essa busca acha o lugar correto

			DADOS_VARIAVEL metadata;
			if(mapaSubstituicaoDeTipoProvisorio.find(tipoProvisorio) == mapaSubstituicaoDeTipoProvisorio.end()){

				while(escopo >= 0){
					metadata = recuperarDadosVariavel(id, escopo);
					escopo = metadata.escopo;
					if(metadata.tipo == "")
					{
						metadata.tipo = tipo;
						atualizarNoMapa(metadata, escopo);
						break;
					}
				}
				/*
				constanteMarcacao = constante_subst_tipo_declaracao_variavel;

				tipoProvisorio = constanteMarcacao.replace(constanteMarcacao.find(separador), separador.length(), idPrefixado);
				sufixoEscopo = constante_sufixo_escopo;
				tipoProvisorio = tipoProvisorio.replace(tipoProvisorio.find(separador), separador.length(), sufixoEscopo + to_string(escopo));
				*/
				tipoProvisorio = recuperarNomeTraducao(id, escopo);
			}

			if(tipo == constante_tipo_string)
			{
				string charArray;
				if(ehDinamica)
					charArray = "char * " + tipoProvisorio;

				else
					charArray = "char " + tipoProvisorio + "[" + to_string(tamanho) + "]";

				mapaSubstituicaoDeTipoProvisorio[tipoProvisorio] = charArray;
			}
			else if(tipo == constante_tipo_array)
			{
				string varArray;
				string asteristico = " *";
				string asteristicoDuplo = " **";

				if(tipoArray == constante_tipo_inteiro)
					varArray = constante_tipo_inteiro + asteristico + tipoProvisorio;

				if(tipoArray == constante_tipo_flutuante)
					varArray = constante_tipo_flutuante + asteristico + tipoProvisorio;

				if(tipoArray == constante_tipo_booleano)
					varArray = constante_tipo_inteiro + asteristico + tipoProvisorio;

				if(tipoArray == constante_tipo_caracter)
					varArray = constante_tipo_caracter + asteristico + tipoProvisorio;

				if(tipoArray == constante_tipo_string)
					varArray = constante_tipo_caracter + asteristicoDuplo + tipoProvisorio;

				mapaSubstituicaoDeTipoProvisorio[tipoProvisorio] = varArray;
			}
			else
			{
				mapaSubstituicaoDeTipoProvisorio[tipoProvisorio] = tipo;
			}



		}

		string substituirTodasAsDeclaracoesProvisorias(string declaracoes)
		{
			for(map<string, string>::iterator it=mapaSubstituicaoDeTipoProvisorio.begin();it!=mapaSubstituicaoDeTipoProvisorio.end(); ++it)
			{
				string key = it->first;
				string value = it->second;

				if(value == "") continue;

				int pos = declaracoes.find(key);

				if(pos >= 0)
				{

					/*infelizmente não tem como fazer verificação com a string, pois o value não é constante_tipo_string
					 e sim uma combinação do nome da váriável e seu respectivo tamanho.
					Por isso a verificação de todos os outros tipos em lugar disso.*/
					if(value == " "){
						string id = recuperarNome(key);
						string prefix = prefixo_variavel_usuario;
						string params[1] = {id.replace(0, prefix.length(), "")};
						MensagensDeErro::yywarning(MensagensDeErro::montarMensagemDeErro(MSG_WARINING_VARIAVEL_DECLARADA_NAO_UTILIZADA ,params, 1));

						value = constante_tipo_inteiro;
					}

					string intBarrat = constante_tipo_inteiro;
					intBarrat = "\t" + intBarrat;

					if(value == constante_tipo_inteiro || value == constante_tipo_flutuante || value == constante_tipo_caracter || value == constante_tipo_booleano || value == intBarrat)
					{
						declaracoes.replace(pos, key.length(), value);
					}
					else
					{
						declaracoes.replace(pos, calcularTamanhoSubstituicao(pos, declaracoes), value);

					}
					mapaSubstituicaoDeTipoProvisorio[key] = "";

				}
			}

// tinhas posto o clear pra ajudar no gerenciamento de chaves repetidas... mas deu problema
//			mapaSubstituicaoDeTipoProvisorio.clear();
			if(numeroEscopoAtual == 0 && mapaSubstituicaoDeTipoProvisorio.size() > 0){

			}
			return declaracoes;
		}
	}

	namespace VariaveisTemporarias{
		#define prefixo_variavel_sistema "temp"

		string gerarNovaVariavel();
		//declaração de variaveis var <nome variavel>;
		string gerarNovaVariavel(){
			static int num = 0;
			num++;
			string temp = prefixo_variavel_sistema;

			string numInt = to_string(num);
			return temp + numInt;
		}
	}


	using namespace VariaveisTemporarias;
	using namespace MapaDeContexto;
	using namespace DeclaracaoProvisoriaInferenciaTipo;
	using namespace ControleSubstituicaoDeVariaveis;

}
