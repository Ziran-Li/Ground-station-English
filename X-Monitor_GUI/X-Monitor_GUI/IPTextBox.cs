using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Controls;
using System.Windows.Input;
using System.Text.RegularExpressions;

namespace TestIPBox.Controls.Class
{
    class IPTextBox : TextBox
    {
        IPTextBox leftBox = null;
        IPTextBox rightBox = null;

        //�����ھ�
        public void setNeighbour(IPTextBox left, IPTextBox right)
        {
            leftBox = left;
            rightBox = right;
        }

        // ���¼�
        protected override void OnPreviewKeyDown(KeyEventArgs e)
        {
            base.OnPreviewKeyDown(e);

            // ɾ����
            if (e.Key == Key.Back)
            {
                if ((CaretIndex == 0) && (leftBox != null) && SelectionLength == 0)
                {
                    leftBox.Focus();
                    leftBox.CaretIndex = leftBox.Text.Length;
                    e.Handled = true;
                }
            }

            // �������
            if (e.Key == Key.Left)
            {
                if ((CaretIndex == 0) && (leftBox != null))
                {
                    leftBox.Focus();
                    leftBox.CaretIndex = leftBox.Text.Length;
                    e.Handled = true;
                }
            }

            // �������
            if (e.Key == Key.Right)
            {
                if ((CaretIndex == Text.Length) && (rightBox != null))
                {
                    rightBox.Focus();
                    rightBox.CaretIndex = 0;
                    e.Handled = true;
                }
            }
        }

        // �ı�����ʱ
        protected override void OnPreviewTextInput(TextCompositionEventArgs e)
        {
            base.OnPreviewTextInput(e);

            char ch = char.Parse(e.Text);

            if (ch == '.')
            {
                if ((CaretIndex == Text.Length) && (rightBox != null) && Text.Length > 0)
                {
                    rightBox.Focus();
                    rightBox.SelectAll();
                    e.Handled = true;
                    return;
                }
            }
            if (ch >= 19968 && ch <= 40869)
            {
                Text = Text.Replace(e.Text, string.Empty);
                base.SelectionStart = Text.Length;
                e.Handled = true;
                return;
            }
            if (ch < '0' || ch > '9')
            {
                e.Handled = true;
                return;
            }
            if ((Text.Length >= 3) && SelectionLength == 0)
            {
                e.Handled = true;
                return;
            }
        }

        // �ı�����
        protected override void OnTextInput(TextCompositionEventArgs e)
        {
            base.OnTextInput(e);

            int ip = Int16.Parse(Text);
            if (ip > 255)
                Text = "255";

            //01 -> ȥ���׸�0
            string pattern = @"^0\d+$";
            if (Regex.IsMatch(this.Text, pattern))
            {
                base.Text = base.Text.Substring(1, base.Text.Length - 1);
            }
            base.SelectionStart = base.Text.Length;

            if (Text.Length == 3)
            {
                if ((CaretIndex == Text.Length) && (rightBox != null))
                {
                    rightBox.Focus();
                    rightBox.SelectAll();
                }
            }
        }
    }
}
