using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace FreeCellCalculate
{
    //洗牌 shuffle
    //红桃 heart
    //梅花 club
    //方块 diomand
    //黑桃 spade
   

    public class Card
    {
        enum eType
        {
            eHeart = 0,
            eClub,
            eDiamond,
            eSpade,

            eMax,
        }

        public eType    m_type;
        public uint     m_number;
    }
       

    public partial class Form1 : Form
    {
        List<Card> m_cardArray;

        public Form1()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            Calculate();
        }

        private void Calculate()
        {

        }
    }
}
