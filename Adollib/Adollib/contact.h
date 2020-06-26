#pragma once
#include <vector>
#include <assert.h>
#include "quaternion.h"
#include "matrix.h"
#include "rigit_body.h"
#include "physics_global.h"

namespace Adollib {
	//Contactクラス用のnamespace 他の人があまり触れないように
	namespace Contacts {

		//拘束
		struct Constraint {
			vector3 axis;		//拘束軸
			float jacDiagInv;	//拘束式の分母	
			float rhs;			//初期拘束力
			float tangent_rhs;  //拘束軸が法線の面上での拘束力
			float lowerlimit;	//拘束力の下限
			float upperlimit;	//拘束力の上限
			float accuminpulse; //累積される拘束力
		};


		//衝突点情報
		struct Contactpoint {
			float distance;			//衝突深度
			vector3 point[2];		//衝突点
			vector3 normal;			//衝突点の法線ベクトル
			Constraint constraint[3];  //拘束

			//初期化
			void reset() {
				constraint[0].accuminpulse = 0.0f;
				constraint[1].accuminpulse = 0.0f;
				constraint[2].accuminpulse = 0.0f;
			}
		};

		//衝突処理用
		class Contact {
		public:
			int contact_num = 0;    //衝突点の数
			float friction;		//その衝突の摩擦力
			Contactpoint contactpoints[Contact_max_per_pair]; //衝突点情報

			Contact() {
			}

			//::::::::
			// 衝突点の追加
			// float penetration : 貫通量
			// vector3 normal : 衝突法線 (ワールド座標系)
			// vector3 contactpointA : 衝突点 (point[0]のローカル座標系)
			// vector3 contactpointB : 衝突点 (point[1]のローカル座標系)
			//::::::::
			void addcontact(
				float penetration,
				const vector3& normal,
				const vector3& contact_pointA,
				const vector3& contact_pointB
			);

			//:::::::
			// 衝突点の衝突が今も存在するかの確認
			// vector3 pointA : Rigitbody[0]の位置
			// vector3 rotA : Rigitbody[0]の姿勢	
			// vector3 pointB : Rigitbody[1]の位置	
			// vector3 rotB : Rigitbody[1]の姿勢	
			//:::::::
			void chack_remove_contact_point(
				const vector3& pointA,
				const quaternion& rotA,
				const vector3& pointB,
				const quaternion& rotB
			);
		private:
			//:::::::::
			// addcontactで使用
			// 除去するContactPointを見つける
			// vector3 newPoint : 追加するCPの新しい衝突点
			// float newDistance : 追加するCPの新しい貫通量
			//:::::::::
			int find_exclusion_point(const vector3& newPoint, float newDistance);

			//::::::::::
			// find_exclusion_pointで使用
			// 4点で作られる4角形の面積^2を返す
			//::::::::::
			float area_4points(const vector3& p1, const vector3& p2, const vector3& p3, const vector3& p4);

			//::::::::
			// i 番目の衝突点を破棄 その後詰める
			//::::::::
			void remove_contactpoint(int i);

			//::::::::
			// すでにその衝突点が作られていた場合contactpointの番号を返す
			// 作られていなければ-1を返す
			// vector3 contactpointA : 衝突点 (point[0]のローカル座標系)
			// vector3 contactpointB : 衝突点 (point[1]のローカル座標系)
			// vector3 normal : 衝突法線 (ワールド座標系)
			//::::::::
			int find_contact_point(
				const vector3& contact_pointA,
				const vector3& contact_pointB,
				const vector3& normal
			);

			//初期化
			void reset() {
				contact_num = 0;
				for (int i = 0; i < Contact_max_per_pair; i++) {
					contactpoints[i].reset();
				}
			};

			void resolve();
		};

		enum  class Pairtype {
			new_pair, // 新規
			keep_pair, // 維持
		};

		struct Contact_pair {
	
			Pairtype type; //衝突の種類(前フレームからある衝突かどうか)

			Collider* body[2]; //接触したobject
			

			Contact contacts; //衝突の情報
		};

	}
}