#include "game.h"
#include <QMessageBox>
#include "qevent.h"
using namespace std;

game::game(int lvl,int avtr)
{
    this->setFixedSize(810,640);
    this->setWindowIcon(QIcon(":/img/menu_icon.png"));
    this->setWindowTitle("New Game");
    level=lvl;

    //background
    QLabel *bg = new QLabel(this);
    bg->setGeometry(0,0,810,640);
    QPixmap *bgP = new QPixmap(":/img/game_bg.png");
    bg->setPixmap(*bgP);

    //initial the game square
    set_game(lvl,avtr);

    //Main Buttons
    hint_game = new QPushButton(this);
    hint_game->setGeometry(640,260,150,40);
    hint_game->setCursor(Qt::PointingHandCursor);
    hint_game->setIcon(QIcon(":/img/hint.png"));
    hint_game->setIconSize(QSize(150,40));
    hint_game->raise();
    QObject::connect(hint_game,SIGNAL(clicked(bool)),this,SLOT(hint_box()));

    solve_game = new QPushButton(this);
    solve_game->setGeometry(640,340,150,40);
    solve_game->setCursor(Qt::PointingHandCursor);
    solve_game->setIcon(QIcon(":/img/solve.png"));
    solve_game->setIconSize(QSize(150,40));
    solve_game->raise();
    QObject::connect(solve_game,SIGNAL(clicked(bool)),this,SLOT(solve()));

    exit_game = new QPushButton(this);
    exit_game->setGeometry(640,420,150,40);
    exit_game->setCursor(Qt::PointingHandCursor);
    exit_game->setIcon(QIcon(":/img/exit_new.png"));
    exit_game->setIconSize(QSize(150,40));
    exit_game->raise();
    QObject::connect(exit_game,SIGNAL(clicked(bool)),this,SLOT(exit_app()));

    QString text("TIMING");
    timing = new QLabel(text,this);
    timing->setGeometry(640,60,150,40);
    QPixmap *tim = new QPixmap(":/img/timing.png");
    timing->setPixmap(*tim);
    timing->raise();

    //Set Music
    GameSound = new QPushButton(this);
    GameSound->setGeometry(720,580,50,40);
    GameSound->setCursor(Qt::PointingHandCursor);
    GameSound->setIcon(QIcon(":/img/on2.png"));
    GameSound->setIconSize(QSize(50,40));
    GameSound->raise();
    QObject::connect(GameSound,SIGNAL(clicked(bool)),this,SLOT(GameSound_button()));
    SoundEffect = new QPushButton(this);
    SoundEffect->setGeometry(650,580,50,40);
    SoundEffect->setCursor(Qt::PointingHandCursor);
    SoundEffect->setIcon(QIcon(":/img/on1.png"));
    SoundEffect->setIconSize(QSize(50,40));
    SoundEffect->raise();
    QObject::connect(SoundEffect,SIGNAL(clicked(bool)),this,SLOT(SoundEffect_button()));

    GameSound_control = new QMediaPlayer();
    GameSound_control->setMedia(QUrl("qrc:/audio/game_sound.mp3"));
    GameSound_control->play();

    MoveSound = new QMediaPlayer();
    MoveSound->setMedia(QUrl("qrc:/audio/move.mp3"));

    ErrorSound = new QMediaPlayer();
    ErrorSound->setMedia(QUrl("qrc:/audio/error.mp3"));

    winSound = new QMediaPlayer();
    winSound->setMedia(QUrl("qrc:/audio/win.mp3"));
    loseSound = new QMediaPlayer();
    loseSound->setMedia(QUrl("qrc:/audio/lose.mp3"));

}

void game::exit_app()
{
    int choix = QMessageBox::question(this,"Exit","You would like to go to the menu ?",QMessageBox::Yes | QMessageBox::No);
       if(choix == QMessageBox::Yes)
       {
           this->close();
           GameSound_control->stop();
           winSound->stop();
           loseSound->stop();
           menu *m = new menu;
           m->show();
       }
       else if(choix == QMessageBox::No)
            qApp->quit();
}
void game::hint_box()
{
    if(solution[x_position][y_position]==1 && hint_number>0)
            QMessageBox::information(this,"Hint Box","You are on the right path ^_^");
    else
            QMessageBox::information(this,"Hint Box","You are on the wrong path :-(");

    hint_number--;
    if(hint_number==0)
        hint_game->setEnabled(0);

    (*square)->setFocus();

}

void game::solve()
{
    if(gameMute==0)
    {
       GameSound_control->stop();
       loseSound->setPosition(0);
       loseSound->play();
    }
    solve_game->setEnabled(0);
    hint_game->setEnabled(0);
    QPixmap *key;
    switch(level)
    {
    case 0: {key = new QPixmap(":/img/solution0.png");break;}
    case 1: {key = new QPixmap(":/img/solution1.png");break;}
    case 2: {key = new QPixmap(":/img/solution2.png");break;}
    }

    for(int i=0;i<dim;i++)
        for(int j=0;j<dim;j++)
            if((i!=x_position || j!=y_position) && (i!=dim-1 || j!=dim-1) && solution[i][j]==1)
                square[i][j].setPixmap(*key);


}
void game::GameSound_button()
{
    if(gameMute == 0)
    {
        gameMute=1;
        GameSound_control->pause();
        GameSound->setIcon(QIcon(":/img/off2.png"));
    }
    else
    {
        gameMute=0;
        GameSound_control->play();
        GameSound->setIcon(QIcon(":/img/on2.png"));
    }
    (*square)->setFocus();
}
void game::SoundEffect_button()
{
    if(effectMute==0)
    {
        effectMute=1;
        SoundEffect->setIcon(QIcon(":/img/off1.png"));
    }
    else
    {
        effectMute=0;
        SoundEffect->setIcon(QIcon(":/img/on1.png"));
    }
    (*square)->setFocus();
}

void game::set_game(int lvl, int avtr)
{
    switch(lvl)
    {
        case 0:{dim=10;maze = new SideWinder(dim,dim);hint_number=1;break;}
        case 1:{dim=18;maze = new Prim(dim,dim);hint_number=2;break;}
        case 2:{dim=25;maze = new growing_tree(dim,dim);hint_number=3;break;}
    }

    //set empty Pixmap : switching in term of cells
    empty = new QPixmap("/");

    //allocate to the cells
    square = new QLabel* [dim];
    for(int i=0;i<dim;i++)
        square[i] = new QLabel[dim];
    //we should apply layout only on the game place so we create a widget for that
    game_space = new QWidget(this);
    game_space->setGeometry(20,20,600,600);
    game_space->setStyleSheet("background-color: #e5e5e5");
    game_layout = new QGridLayout;
    game_layout->setContentsMargins(QMargins(0,0,0,0)); //layout margin
    game_layout->setSpacing(0); //space between the cells
    //add labels to the grid
    for(int i=0;i<dim;i++)
    {
        for(int j=0;j<dim;j++)
        {
            game_layout->addWidget(&square[i][j],i,j);
            square[i][j].setFrameShape(QFrame::NoFrame);
            square[i][j].setAlignment(Qt::AlignCenter);
            //square[i][j].setStyleSheet("border: 1px solid black;");

        }
    }
    game_space->setLayout(game_layout);
    generate_maze();

    //set the avatar and the flag
    if(lvl==0 && avtr==0){
        avatar = new QPixmap(":/img/easy0.png");
        square[0][0].setPixmap(*avatar);
        flag = new QPixmap(":/img/flag0.png");
        square[dim-1][dim-1].setPixmap(*flag);
    }
    else if(lvl==1 && avtr==0){
        avatar = new QPixmap(":/img/medium0.png");
        square[0][0].setPixmap(*avatar);
        flag = new QPixmap(":/img/flag1.png");
        square[dim-1][dim-1].setPixmap(*flag);
    }
    else if(lvl==2 && avtr==0){
        avatar = new QPixmap(":/img/hard0.png");
        square[0][0].setPixmap(*avatar);
        flag = new QPixmap(":/img/flag2.png");
        square[dim-1][dim-1].setPixmap(*flag);
    }
    else if(lvl==0 && avtr==1){
        avatar = new QPixmap(":/img/easy1.png");
        square[0][0].setPixmap(*avatar);
        flag = new QPixmap(":/img/flag0.png");
        square[dim-1][dim-1].setPixmap(*flag);
    }
    else if(lvl==1 && avtr==1){
        avatar = new QPixmap(":/img/medium1.png");
        square[0][0].setPixmap(*avatar);
        flag = new QPixmap(":/img/flag1.png");
        square[dim-1][dim-1].setPixmap(*flag);
    }
    else if(lvl==2 && avtr==1){
        avatar = new QPixmap(":/img/hard1.png");
        square[0][0].setPixmap(*avatar);
        flag = new QPixmap(":/img/flag2.png");
        square[dim-1][dim-1].setPixmap(*flag);
    }
    else if(lvl==0 && avtr==2){
        avatar = new QPixmap(":/img/easy2.png");
        square[0][0].setPixmap(*avatar);
        flag = new QPixmap(":/img/flag0.png");
        square[dim-1][dim-1].setPixmap(*flag);
    }
    else if (lvl==1 && avtr==2){
        avatar = new QPixmap(":/img/medium2.png");
        square[0][0].setPixmap(*avatar);
        flag = new QPixmap(":/img/flag1.png");
        square[dim-1][dim-1].setPixmap(*flag);
    }
    else if(lvl==2 && avtr==2){
        avatar = new QPixmap(":/img/hard2.png");
        square[0][0].setPixmap(*avatar);
        flag = new QPixmap(":/img/flag2.png");
        square[dim-1][dim-1].setPixmap(*flag);
    }

    //set solution
    solution = new int* [dim];
    for(int i=0;i<dim;i++)
        solution[i] = new int[dim];
    for(int i=0;i<dim;i++)
        for(int j=0;j<dim;j++)
            solution[i][j]=0;

    while(!maze->chemin.empty())
    {
        int i,j;
        i=maze->chemin.top().first;
        j=maze->chemin.top().second;
        solution[i][j]=1;
        maze->chemin.pop();
    }

    (*square)->setFocus();
}

void game::generate_maze()
{
    //check the open walls for each cell
    for(int i=0;i<dim;i++)
    {
        for(int j=0;j<dim;j++)
        {
            if((maze->cell[i][j]&1)==1)
                square[i][j].setStyleSheet("border-top: 1px dashed #e5e5e5;\n""border-bottom: 1px dashed black;\n""border-left: 1px dashed black;\n""border-right: 1px dashed black;\n");
            if((maze->cell[i][j]&2)==2)
                square[i][j].setStyleSheet("border-top: 1px dashed black;\n""border-bottom: 1px dashed #e5e5e5;\n""border-left: 1px dashed black;\n""border-right: 1px dashed black;\n");
            if((maze->cell[i][j]&3)==3)
                square[i][j].setStyleSheet("border-top: 1px dashed #e5e5e5;\n""border-bottom: 1px dashed #e5e5e5;\n""border-left: 1px dashed black;\n""border-right: 1px dashed black;\n");
            if((maze->cell[i][j]&4)==4)
                square[i][j].setStyleSheet("border-top: 1px dashed black;\n""border-bottom: 1px dashed black;\n""border-left: 1px dashed black;\n""border-right: 1px dashed #e5e5e5;\n");
            if((maze->cell[i][j]&5)==5)
                square[i][j].setStyleSheet("border-top: 1px dashed #e5e5e5;\n""border-bottom: 1px dashed black;\n""border-left: 1px dashed black;\n""border-right: 1px dashed #e5e5e5;\n");
            if((maze->cell[i][j]&6)==6)
                square[i][j].setStyleSheet("border-top: 1px dashed black;\n""border-bottom: 1px dashed #e5e5e5;\n""border-left: 1px dashed black;\n""border-right: 1px dashed #e5e5e5;\n");
            if((maze->cell[i][j]&7)==7)
                square[i][j].setStyleSheet("border-top: 1px dashed #e5e5e5;\n""border-bottom: 1px dashed #e5e5e5;\n""border-left: 1px dashed black;\n""border-right: 1px dashed #e5e5e5;\n");
            if((maze->cell[i][j]&8)==8)
                square[i][j].setStyleSheet("border-top: 1px dashed black;\n""border-bottom: 1px dashed black;\n""border-left: 1px dashed #e5e5e5;\n""border-right: 1px dashed black;\n");
            if((maze->cell[i][j]&9)==9)
                square[i][j].setStyleSheet("border-top: 1px dashed #e5e5e5;\n""border-bottom: 1px dashed black;\n""border-left: 1px dashed #e5e5e5;\n""border-right: 1px dashed black;\n");
            if((maze->cell[i][j]&10)==10)
                square[i][j].setStyleSheet("border-top: 1px dashed black;\n""border-bottom: 1px dashed #e5e5e5;\n""border-left: 1px dashed #e5e5e5;\n""border-right: 1px dashed black;\n");
            if((maze->cell[i][j]&11)==11)
                square[i][j].setStyleSheet("border-top: 1px dashed #e5e5e5;\n""border-bottom: 1px dashed #e5e5e5;\n""border-left: 1px dashed #e5e5e5;\n""border-right: 1px dashed black;\n");
            if((maze->cell[i][j]&12)==12)
                square[i][j].setStyleSheet("border-top: 1px dashed black;\n""border-bottom: 1px dashed black;\n""border-left: 1px dashed #e5e5e5;\n""border-right: 1px dashed #e5e5e5;\n");
            if((maze->cell[i][j]&13)==13)
                square[i][j].setStyleSheet("border-top: 1px dashed #e5e5e5;\n""border-bottom: 1px dashed black;\n""border-left: 1px dashed #e5e5e5;\n""border-right: 1px dashed #e5e5e5;\n");
            if((maze->cell[i][j]&14)==14)
                square[i][j].setStyleSheet("border-top: 1px dashed black;\n""border-bottom: 1px dashed #e5e5e5;\n""border-left: 1px dashed #e5e5e5;\n""border-right: 1px dashed #e5e5e5;\n");
            if((maze->cell[i][j]&15)==15)
                square[i][j].setStyleSheet("border: 1px dashed #e5e5e5;");
        }
    }

}
void game::keyPressEvent ( QKeyEvent * event)
{

      if(event->key() == Qt::Key_Right)
    {
          if((maze->cell[x_position][y_position]&4)==4)
          {
           square[x_position][y_position].setPixmap(*empty);
           square[x_position][++y_position].setPixmap(*avatar);
               if(effectMute==0)
               {
                   MoveSound->setPosition(0);
                   MoveSound->play();
               }

          }
          else
          {
              if(effectMute==0)
              {
                  ErrorSound->setPosition(0);
                  ErrorSound->play();
              }

          }

    }
    if(event->key() == Qt::Key_Left)
    {
        if((maze->cell[x_position][y_position]&8)==8)
        {
        square[x_position][y_position].setPixmap(*empty);
        square[x_position][--y_position].setPixmap(*avatar);
            if(effectMute==0)
            {
                MoveSound->setPosition(0);
                MoveSound->play();
            }
        }
        else
        {
            if(effectMute==0)
            {
                ErrorSound->setPosition(0);
                ErrorSound->play();
            }

        }

    }
    if(event->key() == Qt::Key_Up)
    {
        if((maze->cell[x_position][y_position]&1)==1)
        {
        square[x_position][y_position].setPixmap(*empty);
        square[--x_position][y_position].setPixmap(*avatar);
            if(effectMute==0)
            {
                MoveSound->setPosition(0);
                MoveSound->play();
            }
        }
        else
        {
            if(effectMute==0)
            {
                ErrorSound->setPosition(0);
                ErrorSound->play();
            }

        }


    }
    if(event->key() == Qt::Key_Down)
    {
        if((maze->cell[x_position][y_position]&2)==2)
        {
        square[x_position][y_position].setPixmap(*empty);
        square[++x_position][y_position].setPixmap(*avatar);
            if(effectMute==0)
            {
                MoveSound->setPosition(0);
                MoveSound->play();
            }
        }
        else
        {
            if(effectMute==0)
            {
                ErrorSound->setPosition(0);
                ErrorSound->play();
            }

        }

    }
    else
      event->ignore();



    if(x_position==dim-1 && y_position==dim-1) win();

}



void game:: win()
{
     if(gameMute==0)
     {
        GameSound_control->stop();
        winSound->setPosition(0);
        winSound->play();
     }
     QMessageBox::information(this,"GAME OVER","Game Over You win");
     int choix = QMessageBox::question(this,"Replay","Choose your level and replay  ?",QMessageBox::Yes | QMessageBox::No);
        if(choix == QMessageBox::Yes)
        {
            this->close();
            winSound->stop();
            loseSound->stop();
            menu *m = new menu;
            m->show();
        }
        else if(choix == QMessageBox::No)
             qApp->quit();
}
