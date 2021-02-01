/*
Nome: Luiz Henrique Göpfert Palhano Leal
DRE: 118061663
CompConc 2020.1
*/

//tarefa a ser executada pelos produtores
class ProducerTask extends Thread {
    private int id;
    private int executions;
    Buffer buffer;

    public ProducerTask(int i, int executions, Buffer buffer) {
        this.id = i;
        this.executions = executions;
        this.buffer = buffer;
    }

    public void run() {
        for (int i = 0; i<executions; i++) {
            System.out.println("Iniciando thread produtora");
            buffer.insertElement(id);

            try {
                this.sleep(100);
            } catch (InterruptedException e) {
                return;
            }

            buffer.printBuffer();
        }
    }

}

//tarefa a ser executada pelos consumidores
class ConsumerTask extends Thread {
    private int executions;
    private Buffer buffer;

    public ConsumerTask(int executions, Buffer buffer) {
        this.executions = executions;
        this.buffer = buffer;
    }

    public void run() {
        System.out.println("Iniciando thread consumidora");
        for (int i = 0; i<executions; i++) {
            buffer.removeElement();

            try {
                this.sleep(100);
            } catch (InterruptedException e) {
                return;
            }

            buffer.printBuffer();
        }
    }
}

public class Buffer {

    //buffer de inteiros
    private final int BUFFER_CAPACITY = 100;
    private int currentLoad;
    private int nextPosition;
    private int lastPosition;
    private int[] buffer;

    //construtor da classe. Inicializa o buffer
    public Buffer() {
        this.currentLoad = 0;
        this.nextPosition = 0;
        this.lastPosition = 0;
        this.buffer = new int[BUFFER_CAPACITY];
    }

    //exibe o buffer
    public synchronized void printBuffer() {
        //se o buffer está vazio, não exibe nada
        if (currentLoad == 0) {
            System.out.println("O buffer está vazio.");
            return;
        }

        //se o buffer não está vazio, exibe o buffer
        int i = lastPosition;
        int aux = 0;
        System.out.println("Imprimindo elementos do buffer:");
        while(aux < currentLoad) {
            System.out.println("Elemento " + buffer[i] + " na posição " + i);
            i = (i+1)%BUFFER_CAPACITY;
            aux++;
        }

    }

    //insere um elemento na próxima posição disponível, se houver uma
    public synchronized void insertElement(int element) {
        //se o buffer estiver cheio, bloqueia
        try {
            while (currentLoad == BUFFER_CAPACITY) {
                System.out.println("Bloqueando a inserção (buffer cheio)");
                wait();
            }
        } catch (InterruptedException e) {
            return;
        }

        //se o buffer não estiver cheio, insere
        buffer[nextPosition] = element;
        System.out.println("Elemento " + element + " inserido na posição " + nextPosition);
        currentLoad++;
        System.out.println("Carga atual: " + currentLoad);
        nextPosition = (nextPosition+1)%BUFFER_CAPACITY;
        notifyAll();
    }

    //retorna o elemento mais antigo do buffer, se ele não estiver vazio
    public synchronized int removeElement() {
        //se o buffer estiver vazio, bloqueia
        try {
            while (currentLoad == 0) {
                System.out.println("Bloqueando a remoção (buffer vazio)");
                wait();
            }
        } catch (InterruptedException e) {
            return -1;
        }

        //se o buffer não estiver vazio, remove
        int aux = lastPosition;
        lastPosition = (lastPosition+1)%BUFFER_CAPACITY;
        System.out.println("Elemento" + buffer[aux] + " retornado da posição " + aux);
        currentLoad--;
        System.out.println("Carga atual: " + currentLoad);
        notifyAll();
        return buffer[aux];
    }

    //main
    public static void main(String[] args) {
        final int NTHREADS = 4;
        final int NEXECUTIONS = 100;
        Buffer buffer = new Buffer();
        Thread[] threads = new Thread[NTHREADS];
        int i;

        //cria as threads
        for (i = 0; i<NTHREADS; i++) {
            if (i%2 == 0) {
                System.out.println("Criando thread produtora");
                threads[i] = new ProducerTask(i, NEXECUTIONS, buffer);
            }
            else {
                System.out.println("Criando thread consumidora");
                threads[i] = new ConsumerTask(NEXECUTIONS, buffer);
            }
        }

        //dispara as threads
        for (i = 0; i<NTHREADS; i++) {
            threads[i].start();
        }

        //espera o retorno das threads
        for (i = 0; i<threads.length; i++) {
            try {
                threads[i].join();
            } catch (InterruptedException e) {
                return;
            }
        }

        System.out.println("Fim da execução.");
    }
}
