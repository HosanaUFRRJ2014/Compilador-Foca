#include<iostream>
#include <string>
#include <sstream>
#include <map>
#include <utility>


using namespace ControleDeVariaveis;
using namespace MapaTipos;
//using namespace VariaveisTemporarias;

namespace EntradaESaida
{

	string constroiPrint(string);
	string constroiScan(string);

	string constroiPrint(string label)
	{
		string print = "\tstd::cout << " + label + " << "+ " \"\\n\"" +";\n\n";
		return print;
	}


	string constroiScan(string label)
	{
		string scan = "\tstd::cin >> " + label + ";\n"; // + ">>"+ " \"\\n\"" +";\n\n";
		return scan;
	}
	
	string construirTraducaoEntrada(string dolarDolar, string label1, string tipo, int tamanho)
	{
		//$1.tipo = constante_tipo_inteiro;
		dolarDolar = gerarNovaVariavel();
		adcionarDefinicaoDeTipo(label1, tipo, tamanho);
		string traducaoDeclaracaoDeVariaveis = "\t"  + tipo + " " + dolarDolar + ";\n";
		
		return traducaoDeclaracaoDeVariaveis;
	
	}
	
	
/*	string construirTraducaoEntrada(string labelDolarDolar, string label1, string tipo, int tamanho)
	{
		//$1.tipo = constante_tipo_inteiro;
		labelDolarDolar = gerarNovaVariavel();
		string traducaoDeclaracaoDeVariaveis = "\t"  + tipo + " " + labelDolarDolar + ";\n";
		adcionarDefinicaoDeTipo(label1, tipo, tamanho);
		
		return traducaoDeclaracaoDeVariaveis;
	
	}*/

}