package krbserver;

import java.io.File;
import java.security.PrivilegedActionException;
import java.util.Scanner;

import javax.security.auth.login.LoginException;

import org.infinispan.arquillian.core.InfinispanResource;
import org.infinispan.arquillian.core.RemoteInfinispanServer;
import org.infinispan.server.test.client.hotrod.security.HotRodSaslAuthTestBase;
import org.infinispan.test.integration.security.utils.ApacheDsKrbLdap;

import java.rmi.Remote;
import java.rmi.RemoteException;
import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;
import java.rmi.server.UnicastRemoteObject;

interface Killable extends Remote {
	void kill() throws RemoteException;
}

public class Server extends HotRodSaslAuthTestBase implements Killable {

	private static final String KRB_REALM = "INFINISPAN.ORG";
	private static final int rmiPort = 51099;
	private static ApacheDsKrbLdap krbLdapServer;
	@InfinispanResource("hotrodAuthKrb")
	RemoteInfinispanServer server;

	public static void main(String[] args) {

		if (args.length < 1 || !args[0].equals("stop")) {
			try {
				LocateRegistry.createRegistry(rmiPort);
				Server obj = new Server();
				Killable stub = (Killable) UnicastRemoteObject.exportObject(obj, 0);

				// Bind the remote object's stub in the registry
				Registry registry = LocateRegistry.getRegistry(rmiPort);
				registry.bind("krbServer", stub);
				krbLdapServer = new ApacheDsKrbLdap("localhost");
				krbLdapServer.start();
			} catch (Exception e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		} else {
			try {
				Registry registry = LocateRegistry.getRegistry(rmiPort);
				Killable stub = (Killable) registry.lookup("krbServer");
				stub.kill();
			} catch (Exception e) {
			}
		}
	}

	@Override
	public RemoteInfinispanServer getRemoteServer() {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public String getTestedMech() {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public void initAsAdmin() throws PrivilegedActionException, LoginException {
		// TODO Auto-generated method stub

	}

	@Override
	public void initAsReader() throws PrivilegedActionException, LoginException {
		// TODO Auto-generated method stub

	}

	@Override
	public void initAsSupervisor() throws PrivilegedActionException, LoginException {
		// TODO Auto-generated method stub
	}

	@Override
	public void initAsWriter() throws PrivilegedActionException, LoginException {
		// TODO Auto-generated method stub

	}

	@Override
	public void kill() throws RemoteException {
		System.exit(0);
	}

}
