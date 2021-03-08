#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <Windows.h>

struct Lbrary
{
	std::vector<std::string> NameBook;
	std::vector<std::string> AuthorBook;
	std::vector<std::string> GenreBook;
};

const char* SigOfFile = "%PDF\0";
const int KolvoChars = 256;
const int MinLen = 5;

int beg_check(Lbrary& Letibrary);
int input(std::string& Command, std::string& NamePath);
int mode_find(const Lbrary Letibrary, std::vector<int>& OutputSearch);
int find_file(const std::string NamePath, Lbrary& Letibrary);
int add_book(const std::filesystem::path NamePath, Lbrary& Letibrary);
int find_book(const std::vector<std::string> AreaSearch, const std::string NameSearch, std::vector<int>& OutputSearch);
int edit_book(Lbrary& Letibrary);
int del_book(Lbrary& Letibrary);
int sort_book(Lbrary& Letibrary, int Left, int Right);
int help_book();
int output_books_to_screen(Lbrary& Letibrary);
int output_books_to_file(Lbrary& Letibrary);

int main()
{
	setlocale(LC_ALL, "Rus");
	std::string Command;
	std::string NamePath;
	Lbrary Letibrary;
	beg_check(Letibrary);
	do
	{
		input(Command, NamePath);
		if (Command == "add") find_file(NamePath, Letibrary);
		else if (Command == "find")
		{
			std::vector<int> OutputSearch;
			mode_find(Letibrary, OutputSearch);
			if (!OutputSearch.empty())
			{
				int SzOutSearch = OutputSearch.size();
				for (size_t i = 0; i < SzOutSearch; ++i)
				{
					char ChooseOpen = 0;
					int NumStr = OutputSearch.at(i);
					do
					{
						std::cout << "�������: " << Letibrary.NameBook.at(NumStr) << "   " << Letibrary.AuthorBook.at(NumStr) << "   " << Letibrary.GenreBook.at(NumStr) << "\n������ �������? (Y/N)\n";
						std::cin >> ChooseOpen;
						if ((ChooseOpen == 'Y') || (ChooseOpen == 'y'))
						{
							std::string FindBook = "cd Books && " + Letibrary.NameBook.at(NumStr) + ".pdf" + " && cd ..";
							system(FindBook.c_str());
						}
						else if (!((ChooseOpen == 'N') || (ChooseOpen == 'n')))
						{
							ChooseOpen = 0x20;
							std::cin.ignore();
						}
					} while (ChooseOpen == ' ');
				}
			}
		}
		else if (Command == "edit") edit_book(Letibrary);
		else if (Command == "delete") del_book(Letibrary);
		else if (Command == "help") help_book();
		else if (Command == "clear") system("cls");
		else if (Command == "output")
		{
			sort_book(Letibrary, 0, Letibrary.NameBook.size() - 1);
			output_books_to_screen(Letibrary);
		}
		else if (Command == "save")
		{
			if (output_books_to_file(Letibrary) == 0)
				std::cout << "���������!\n";
		}
		else if (Command != "exit") std::cout << "�������� �������, ��������� �������\n";
		std::cin.ignore();
	} while (Command != "exit");
	output_books_to_file(Letibrary);
	getchar();
	getchar();
	return 0;
}

int beg_check(Lbrary& Letibrary)
{
	try
	{
		std::filesystem::create_directory("Books");
	}
	catch (...) { ; }
	std::string LbryPath = "LETIBRARY.txt";
	std::ifstream Input;
	Input.open(LbryPath, std::ios_base::in);
	if (!Input.is_open())
	{
		std::cerr << "Error 2\n";
		return 2;
	}
	if (!std::filesystem::is_empty(LbryPath))
	{
		while (!Input.eof())
		{
			std::string tmp;
			Input >> tmp;
			if (tmp.size() > 0)
			{
				Letibrary.NameBook.push_back(tmp);
				tmp.clear();
				Input >> tmp;
				Letibrary.AuthorBook.push_back(tmp);
				tmp.clear();
				Input >> tmp;
				Letibrary.GenreBook.push_back(tmp);
				tmp.clear();
			}
		}
	}
	Input.close();
	return 0;

}

int input(std::string& Command, std::string& NamePath)
{
	std::string Inpt;
	int SzInpt = 0;
	bool Spce = 0;
	int j = 0;
	do
	{
		if (j != 0)
		{
			if (!std::filesystem::exists(NamePath)) std::cout << "�������� ���� �� ����������, ������� ���������� ����\n";
		}
		Inpt.clear();
		Command.clear();
		NamePath.clear();
		std::cout << "������� �������, ��� ������������� �������� ������� �������� \"help\"\n";
		std::getline(std::cin, Inpt);
		SzInpt = Inpt.size();
		for (size_t i = 0; i < SzInpt; ++i)
		{
			if (Spce)
			{
				NamePath.push_back(Inpt.at(i));
				++j;
			}
			if ((Inpt.at(i) == ' ') && (!Spce)) Spce = 1;
			if (!Spce) Command.push_back(Inpt.at(i));
		}
		Spce = 0;
	} while ((!std::filesystem::exists(NamePath)) && (Command == "add"));
	return 0;
}

int find_file(const std::string NamePath, Lbrary& Letibrary)
{
	if (std::filesystem::is_directory(NamePath))
	{
		for (const auto& EntryFile : std::filesystem::recursive_directory_iterator(NamePath))
		{
			std::filesystem::directory_entry AdrFile = EntryFile;
			if (!std::filesystem::is_directory(AdrFile)) add_book(AdrFile.path(), Letibrary);
		}
	}
	else add_book(NamePath, Letibrary);
	return 0;
}

int add_book(const std::filesystem::path AdrFile, Lbrary& Letibrary)
{
	std::ifstream SelectBook;
	char* tmp;
	tmp = new char[5];
	SelectBook.open(AdrFile, std::ios_base::in | std::ios_base::binary);
	if (!SelectBook.is_open())
	{
		std::cerr << "Error 3\n";
		delete[] tmp;
		return 3;
	}
	SelectBook.read(tmp, 4);
	tmp[4] = '\0';
	SelectBook.close();
	if (strcmp(tmp, SigOfFile) == 0)
	{
		size_t i = 0;
		int SzAdr = 0;
		bool tochka = 0;
		std::filesystem::path FinPath = "Books";
		std::string tmp1 = AdrFile.string();
		SzAdr = tmp1.size();
		try { std::filesystem::copy(AdrFile, FinPath); }
		catch (...) { ; }
		for (i = SzAdr - 1; tmp1.at(i) != '\\'; --i)
		{
			if ((tmp1.at(i) == '.') && (!tochka))
			{
				tmp1.erase(tmp1.begin() + i, tmp1.end());
				tochka = 1;
			}
		}
		++i;
		tmp1.erase(tmp1.begin(), tmp1.begin() + i);
		int Sztmp1 = tmp1.size();
		for (size_t i = 0; i < Sztmp1; ++i) if (tmp1.at(i) == ' ') tmp1.at(i) = '_';
		std::filesystem::rename(AdrFile, tmp1);
		Letibrary.NameBook.push_back(tmp1);
		tmp1.clear();
		std::cout << "������� ������ " << Letibrary.NameBook.back() << " (��� ��������)\n";
		std::cin >> tmp1;
		Letibrary.AuthorBook.push_back(tmp1);
		tmp1.clear();
		std::cout << "������� ���� " << Letibrary.NameBook.back() << " (��� ��������)\n";
		std::cin >> tmp1;
		Letibrary.GenreBook.push_back(tmp1);
	}
	delete[] tmp;
	return 0;
}

int mode_find(const Lbrary Letibrary, std::vector<int>& OutputSearch)
{
	if (Letibrary.NameBook.size() <= MinLen)
	{
		std::cout << "���������� �����\n";
		return 1;
	}
	int ChooseSearch = 0;
	std::string NameFind;
	do
	{
		std::cout << "�������� �������� ������:\n 1) �� ��������\n 2) �� ������\n 3) �� �����\n 4) ����� �� ������\n";
		std::cin >> ChooseSearch;
		if (ChooseSearch == 1)
		{
			std::cout << "������� ��������: ";
			std::cin >> NameFind;

			find_book(Letibrary.NameBook, NameFind, OutputSearch);
		}
		else if (ChooseSearch == 2)
		{
			std::cout << "������� ������: ";
			std::cin >> NameFind;
			find_book(Letibrary.AuthorBook, NameFind, OutputSearch);
		}
		else if (ChooseSearch == 3)
		{
			std::cout << "������� ����: ";
			std::cin >> NameFind;
			find_book(Letibrary.GenreBook, NameFind, OutputSearch);
		}
	} while ((ChooseSearch < 1) || (ChooseSearch > 4));

	return 0;
}

int find_book(const std::vector<std::string> AreaSearch, const std::string NameSearch,std::vector<int>& OutputSearch)
{
	int SzArea = AreaSearch.size();
	int SzName = NameSearch.size();
	bool IsFind = 0;
	int BadChar[KolvoChars] = { 0 };
	for (size_t i = 0; i < KolvoChars; ++i) BadChar[i] = -1;
	for (size_t i = 0; i < SzName; ++i) BadChar[(int)NameSearch.at(i)] = i;
	for (size_t i = 0; i < SzArea; ++i)
	{
		std::string tmp = AreaSearch.at(i);
		int SzTmp = tmp.size();
		int Sdvig = 0;
		while (Sdvig <= (SzTmp - SzName))
		{
			int j = 0;
			for (j = SzName - 1; (j >= 0 && NameSearch.at(j) == tmp.at(Sdvig + j)); --j);
			if (j < 0)
			{
				IsFind = 1;
				OutputSearch.push_back(i);
				if (Sdvig + SzName < SzTmp) Sdvig = Sdvig + (SzName)-BadChar[tmp.at(Sdvig + SzName)];
				else ++Sdvig;
			}
			else
			{
				int tmp1 = j - BadChar[tmp.at(Sdvig + j)];
				if (tmp1 > 1) Sdvig = Sdvig + tmp1;
				else ++Sdvig;
			}
		}
	}
	if (!IsFind) std::cout << "�� ������ ������� ������ �� �������, ���������� �������� ������� ������\n";
	return 0;
}

int edit_book(Lbrary& Letibrary)
{
	if (Letibrary.NameBook.size() <= MinLen)
	{
		std::cout << "���������� �����\n";
		return 1;
	}
	std::vector<int> OutputSearch;
	std::cout << "������ ������ ������\n";
	mode_find(Letibrary, OutputSearch);
	if (!OutputSearch.empty())
	{
		int SzOutSearch = OutputSearch.size();
		for (size_t i = 0; i < SzOutSearch; ++i)
		{
			char ChooseEdit = 0;
			int NumStr = OutputSearch.at(i);
			do
			{
				std::cout << "�������: " << Letibrary.NameBook.at(NumStr) << "   " << Letibrary.AuthorBook.at(NumStr) << "   " << Letibrary.GenreBook.at(NumStr) << "\n������ ��������? (Y/N)\n";
				std::cin >> ChooseEdit;
				if ((ChooseEdit == 'Y') || (ChooseEdit == 'y'))
				{
					std::string NamePath = "Books\\" + Letibrary.NameBook.at(NumStr) + ".pdf";
					std::string tmp;
					std::cout << "������� ����� ��� (��� ��������) ��� ������� ������, ����� �������� �������\n";
					std::cin.ignore();
					std::getline(std::cin, tmp);
					if (tmp != " ") Letibrary.NameBook.at(NumStr) = tmp;
					tmp.clear();
					std::cout << "������� ������ ������ (��� ��������) ��� ������� ������, ����� �������� ��������\n";
					std::getline(std::cin, tmp);
					if (tmp != " ") Letibrary.AuthorBook.at(NumStr) = tmp;
					tmp.clear();
					std::cout << "������� ����� ���� (��� ��������) ��� ������� ������, ����� �������� �������\n";
					std::getline(std::cin, tmp);
					if (tmp != " ") Letibrary.GenreBook.at(NumStr) = tmp;
				}
				else if (!((ChooseEdit == 'N') || (ChooseEdit == 'n')))
				{
					ChooseEdit = 0x20;
					std::cin.ignore();
				}
			} while (ChooseEdit == ' ');
		}
	}
	return 0;
}

int del_book(Lbrary& Letibrary)
{
	if (Letibrary.NameBook.size() <= MinLen)
	{
		std::cout << "���������� �����\n";
		return 1;
	}
	std::vector<int> OutputSearch;
	std::cout << "������ ������ ������\n";
	mode_find(Letibrary, OutputSearch);
	if (!OutputSearch.empty())
	{
		int SzOutSearch = OutputSearch.size();
		for (size_t i = 0; i < SzOutSearch; ++i)
		{
			char ChooseDel = 0;
			int NumStr = OutputSearch.at(i);
			do
			{
				std::cout << "�������: " << Letibrary.NameBook.at(NumStr) << "   " << Letibrary.AuthorBook.at(NumStr) << "   " << Letibrary.GenreBook.at(NumStr) << "\n������ �������? (Y/N)\n";
				std::cin >> ChooseDel;
				if ((ChooseDel == 'Y') || (ChooseDel == 'y'))
				{
					std::string NamePath = "Books\\" + Letibrary.NameBook.at(NumStr) + ".pdf";
					Letibrary.NameBook.erase(Letibrary.NameBook.begin() + NumStr);
					Letibrary.AuthorBook.erase(Letibrary.AuthorBook.begin() + NumStr);
					Letibrary.GenreBook.erase(Letibrary.GenreBook.begin() + NumStr);
					try
					{
						std::filesystem::remove(NamePath);
					}
					catch (...)
					{
						std::cerr << "Error 2 - ���� ����������� � ��������";
						return 2;
					}
				}
				else if (!((ChooseDel == 'N') || (ChooseDel == 'n')))
				{
					ChooseDel = 0x20;
					std::cin.ignore();
				}
			} while (ChooseDel == ' ');
		}
	}
	return 0;
}

int sort_book(Lbrary& Letibrary, int Left, int Right)
{
	if (Letibrary.NameBook.size() <= MinLen)
	{
		std::cout << "���������� �����\n";
		return 1;
	}
	int i = Left;
	int j = Right;
	int Opora = (int)Letibrary.NameBook.at((Left + Right) / 2).at(0);
	while (i <= j)
	{
		while ((int)Letibrary.NameBook.at(i).at(0) < Opora)
		{
			++i;
		}
		while ((int)Letibrary.NameBook.at(j).at(0) > Opora)
		{
			--j;
		}
		if (i <= j)
		{
			int SumI = (int)Letibrary.NameBook.at(i).at(0);
			int SumJ = (int)Letibrary.NameBook.at(j).at(0);
			int SzI = Letibrary.NameBook.at(i).size();
			int SzJ = Letibrary.NameBook.at(j).size();
			if (SumI == SumJ)
			{
				for (size_t NumBukva = 1; (SumI == SumJ) && ((NumBukva < SzI) && (NumBukva < SzJ)); ++NumBukva)
				{
					int CoefBig = 0;
					if ((Letibrary.NameBook.at(i).at(NumBukva) >= 'A') && (Letibrary.NameBook.at(i).at(NumBukva) <= 'Z')) CoefBig = 'a' - 'A';
					else if (!((Letibrary.NameBook.at(i).at(NumBukva) >= 'a') && (Letibrary.NameBook.at(i).at(NumBukva) <= 'z'))) CoefBig = 'z' + 1;
					SumI = SumI + (int)Letibrary.NameBook.at(i).at(NumBukva) + CoefBig;
					CoefBig = 0;
					if ((Letibrary.NameBook.at(j).at(NumBukva) >= 'A') && (Letibrary.NameBook.at(j).at(NumBukva) <= 'Z')) CoefBig = 'a' - 'A';
					else if (!((Letibrary.NameBook.at(j).at(NumBukva) >= 'a') && (Letibrary.NameBook.at(j).at(NumBukva) <= 'z'))) CoefBig = 'z' + 1;
					SumJ = SumJ + (int)Letibrary.NameBook.at(j).at(NumBukva) + CoefBig;
				}
			}
			if (SumI > SumJ)
			{
				std::string tmp = Letibrary.NameBook.at(i);
				Letibrary.NameBook.at(i) = Letibrary.NameBook.at(j);
				Letibrary.NameBook.at(j) = tmp;
				tmp.clear();
				tmp = Letibrary.AuthorBook.at(i);
				Letibrary.AuthorBook.at(i) = Letibrary.AuthorBook.at(j);
				Letibrary.AuthorBook.at(j) = tmp;
				tmp.clear();
				tmp = Letibrary.GenreBook.at(i);
				Letibrary.GenreBook.at(i) = Letibrary.GenreBook.at(j);
				Letibrary.GenreBook.at(j) = tmp;
			}
			++i;
			--j;
		}
	}
	if (Left < j) sort_book(Letibrary, Left, j);
	if (i < Right) sort_book(Letibrary, i, Right);
	return 0;
}

int help_book()
{
	std::cout << "-----------------\n";
	std::cout << "�������\n ";
	std::cout << "������� add\n  ��������� : \"add *���� � ���������� ��� �����*\"\n  ��������� ���� .pdf ���� ��� ��� .pdf ����� �� �������� � ����������\n\n ";
	std::cout << "������� find\n  ��������� : \"find\"\n  ���� ����� �� ��������, ������ ��� �����\n\n ";
	std::cout << "������� edit\n  ��������� : \"edit\"\n  �������� �������� ����������� (����� ������ �� ��������) ����� � ����������\n\n ";
	std::cout << "������� del\n  ��������� : \"del\"\n  ������� ���� ��� ���������.pdf ������ �� ���������� �� ����������� ����������, �������� � ���������� �������������\n\n ";
	std::cout << "������� clear\n  ��������� : \"clear*\"\n  ������� �����\n\n ";
	std::cout << "������� output\n  ��������� : \"output*\"\n  ������� ���������� ���������� �� �����\n\n ";
	std::cout << "������� save\n  ��������� : \"save*\"\n  ��������� ���������\n\n ";
	std::cout << "������� help\n  ��������� : \"help\"\n  ����� �������\n\n ";
	std::cout << "������� exit\n  ��������� : \"exit*\"\n  ����� �� ���������� � ����������� ���������\n\n";
	std::cout << "����� �������\n";
	std::cout << "-----------------\n";
	return 0;
}

int output_books_to_screen(Lbrary& Letibrary)
{
	int SzBooks = Letibrary.NameBook.size();
	const int SpaceStr = 2;
	if (SzBooks <= MinLen)
	{
		std::cout << "���������� �����\n";
		return 1;
	}
	int NameMax = ((std::string)"��������").size();
	int AuthorMax = ((std::string)"�����").size();
	int tmp = 0;
	for (size_t i = 0; i < SzBooks; ++i)
	{
		tmp = Letibrary.NameBook.at(i).size();
		if (NameMax < tmp) NameMax = tmp;
		tmp = Letibrary.AuthorBook.at(i).size();
		if (AuthorMax < tmp) AuthorMax = tmp;
	}
	tmp = NameMax - ((std::string)"��������").size() + SpaceStr;
	std::cout << "��������";
	for (; tmp >= 0; --tmp) std::cout << " ";
	tmp = AuthorMax - ((std::string)"�����").size() + SpaceStr;
	std::cout << "�����";
	for (; tmp >= 0; --tmp) std::cout << " ";
	std::cout << "����" << "\n\n";
	for (size_t i = 0; i < SzBooks; ++i)
	{
		tmp = NameMax - Letibrary.NameBook.at(i).size() + SpaceStr;
		std::cout << Letibrary.NameBook.at(i);
		for (; tmp >= 0; --tmp) std::cout << " ";
		tmp = AuthorMax - Letibrary.AuthorBook.at(i).size() + SpaceStr;
		std::cout << Letibrary.AuthorBook.at(i);
		for (; tmp >= 0; --tmp) std::cout << " ";
		std::cout << Letibrary.GenreBook.at(i) << "\n";
	}

	return 0;
}

int output_books_to_file(Lbrary& Letibrary)
{
	int SzBooks = Letibrary.NameBook.size();
	if (SzBooks <= MinLen)
	{
		std::cout << "���������� �����\n";
		return 1;
	}
	sort_book(Letibrary, 0, SzBooks - 1);
	std::ofstream Output;
	Output.open("LETIBRARY.txt", std::ios_base::out | std::ios_base::trunc);
	if (!Output.is_open())
	{
		std::cerr << "Error 2\n";
		return 2;
	}
	for (size_t i = 0; i < SzBooks; ++i)
	{
		Output << Letibrary.NameBook.at(i) << " " << Letibrary.AuthorBook.at(i) << " " << Letibrary.GenreBook.at(i) << "\n";
	}
	Output.close();
	return 0;
}
	
/*
������ ��� ������� ���������: ��������, �����, ����
���������� ��� ����������� �� .txt �����, ���� �� �� ����
� � ���� ������, ��������� ������������ ������ ��, � ��� ���������� ������ ������������ ���� ������ ����� ����������

����� ����� ���������� � ������������ ����� �� �� ������� �������� � ������ ������ ���, ��� ����� ��� �� �� �����
� ����� ������ ������������ ����� �������, � ����� ������ � ���� ����� ��� ������, � � ��� � ����� �� ���� �� ��� 

���-�� ���

� �������� ����� ����� ���������� 

����� ������� ����, � ������� ����� ����� ������������ �� �������� �� ���������, ��� �������� ����� ������� �� ���� � �������
� ��� ������ ������������ ������� ����������� �� ������, � ����� ��� � ����� ����� � ���� ������ ������
*/


