/*
Nome: Luiz Henrique Göpfert Palhano Leal
DRE: 118061663
CompConc 2020.1
*/

//task a ser executada pelas threads
class Task extends Thread {
    private int startIndex, endIndex;
    private AddingArray a, b, target;

    public Task (int startIndex, int endIndex, AddingArray a, AddingArray b, AddingArray target) {
        this.startIndex = startIndex;
        this.endIndex = endIndex;
        this.a = a;
        this.b = b;
        this.target = target;
    }

    public void run() {
        int currentTotal;
        for (int i  = startIndex; i < endIndex; i++) {
            currentTotal = a.getElement(i) + b.getElement(i);
            target.insertElement(i, currentTotal);
        }
    }

}

//classe local dos vetores a serem utilizados
class AddingArray {
    private static final int SIZE = 100;
    private int[] array;

    public AddingArray() {
        array = new int[SIZE];
        for (int i = 0; i < SIZE; i++) {
            array[i] = i%5;
        }
    }

    public static int arraySize() {
        return SIZE;
    }

    public void insertElement(int index, int element) {
        array[index] = element;
    }

    public int getElement(int index) {
        return array[index];
    }

    public void showArray() {
        System.out.println("Mostrando o vetor:");
        for (int i = 0; i < SIZE; i++) {
            System.out.print(array[i] + " ");
        }
        System.out.println();
    }
}

//contém a main do laboratório
public class ProjectMain {
    private static final int NTHREADS = 4;

    public static void main() {
        //cria os vetores nas classes apropriadas
        Thread[] threads;
        AddingArray a = new AddingArray();
        AddingArray b = new AddingArray();
        AddingArray c = new AddingArray();
        int threadLoad;
        int startIndex = 0;
        int endIndex = 0;
        int nthreads = NTHREADS;

        //inicializa o vetor threads
        threads = new Thread[nthreads];

        //define quanto trabalho cada thread vai ter
        threadLoad = AddingArray.arraySize()/nthreads;

        //inicializa as threads em si
        for(int i = 0; i < nthreads; i++) {
            endIndex += threadLoad;
            if(endIndex > AddingArray.arraySize()) {
                endIndex = AddingArray.arraySize();
            }
            threads[i] = new Task(startIndex, endIndex, a, b, c);
            startIndex += threadLoad;
        }

        //dispara as threads
        for (int i = 0; i < nthreads; i++) {
            threads[i].start();
        }

        //exibe os resultados
        c.showArray();

        //espera o retorno das threads
        for (int i=0; i<threads.length; i++) {
            try {
                threads[i].join();
            } catch (InterruptedException e) {
                return;
            }
        }

        System.out.println("Fim da execução");
    }
}
