#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<iostream>
#include<string>
#include<cstdlib>
#include<QMessageBox>
#include<QFile>
#include<QStringList>
#include<QCoreApplication>
#include<QTextStream>

class node{
    public:
        char info;
        std::string Word;
        node* ptrs[256];
        node(){
            for(int i=0;i<256;i++){
                ptrs[i]=NULL;
            }
            info=NULL;
            Word="";
        }
};
void insertword(std::string word,int pos,class node * root){
    if(word.length()==pos){
        root->Word=word;
        return;
    }
    if( root-> ptrs[word[pos]]==NULL ){
        node *newnode;
        newnode= new node;
        newnode->info=word[pos];
        root->ptrs[word[pos]]=newnode;
        insertword(word,pos+1,root->ptrs[word[pos]]);
    }
    else
        insertword(word,pos+1,root->ptrs[word[pos]]);
}

bool findword(std::string key,int pos, class node * root){
    if((key != root->Word) && (root->ptrs[key[pos]] != NULL))
        return findword(key,pos+1,root->ptrs[key[pos]]);
    else if(key==root->Word){
        return true;
    }
    return false;
}

std::string suggestions[100];
int i = 0;

void printall(class node * root){
    for(int i=0;i<256;i++)
        if(root->ptrs[i]!=NULL){
            printall(root->ptrs[i]);
        }
    if(root->Word != "") {
        suggestions[i] = root->Word;
        i++;
    }
}

void suggest(std::string key,int pos, class node * root){
    if((key != root->Word) && (root->ptrs[key[pos]] != NULL)){
            suggest(key,pos+1,root->ptrs[key[pos]]);
    }
    else{
            printall(root);
    }
    suggestions[i] = "\0";
}

void clearAndReset() {
    for(auto &str : suggestions) {
        str.clear();
    }
    i = 0;
    suggestions[0] = "\0";
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void searchBrute(QStringList wordList, QString word) {
    for(auto it : wordList) {
        if(it == word) {
            break;
        }
    }
}

QString timeTaken(node * root, QStringList wordList, QString word, std::string toSearch) {
    auto startTrie = std::chrono::steady_clock::now();
    findword(toSearch, 0, root);
    auto endTrie = std::chrono::steady_clock::now();
    auto durationTrie = std::chrono::duration_cast<std::chrono::nanoseconds>(endTrie - startTrie);
    QString out("Time Taken using Trie: ");
    out.append(QString::number(durationTrie.count())).append(" nanoseconds\n");
    auto start = std::chrono::steady_clock::now();
    searchBrute(wordList, word);
    auto stop = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    out.append("Time Taken using BruteForce Method: ");
    out.append(QString::number(duration.count())).append(" microseconds\n");
    return out;
}

void MainWindow::on_pushButton_clicked()
{    QString word = ui->lineEdit->text();
     std::string toSearch = word.toLocal8Bit().constData();
     node *root = new node;
     QFile words("C:\\Users\\Aditi Singh\\Desktop\\DTU sem 3\\DST\\PROJECT\\project\\wordlist.txt");
     words.open(QIODevice::ReadOnly);
     QString content = words.readAll();
     QStringList wordList = content.split("\r\n");
     words.close();
     for(auto wordAdd : wordList) {
         std::string toAdd = wordAdd.toLocal8Bit().constData();
         insertword(toAdd, 0, root);
     }
     if(findword(toSearch, 0, root) == true) {
         QString str = "Word found! Spelling is correct\n";
         str.append(timeTaken(root, wordList, word, toSearch));
         QMessageBox::information(this, "Search", str);
     }
     else {
         suggest(toSearch, 0, root);
         int j = 0;
         std::string toprint = "Word not found ! \nPossible suggestions are: ";
         while(suggestions[j] != "\0") {
             toprint += suggestions[j];
             toprint += ", ";
             j++;
         }
         toprint = toprint.substr(0, toprint.length() - 2);
         toprint += "\n";
         QString str = "";
         str.append(QString::fromStdString(toprint)).append(timeTaken(root, wordList, word, toSearch));
         QMessageBox::information(this, "Search", str);
         clearAndReset();
     }
}
